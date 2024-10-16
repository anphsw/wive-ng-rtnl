###################################################
# Python function wrapper generator
# Copyright jelmer@samba.org 2007-2008
# released under the GNU GPL

package Parse::Pidl::Samba4::Python;

use Exporter;
@ISA = qw(Exporter);

use strict;
use Parse::Pidl qw(warning fatal);
use Parse::Pidl::Typelist qw(hasType resolveType getType mapTypeName expandAlias);
use Parse::Pidl::Util qw(has_property ParseExpr unmake_str);
use Parse::Pidl::NDR qw(GetPrevLevel GetNextLevel ContainsDeferred is_charset_array);
use Parse::Pidl::CUtil qw(get_value_of get_pointer_to);
use Parse::Pidl::Samba4 qw(ArrayDynamicallyAllocated);
use Parse::Pidl::Samba4::Header qw(GenerateFunctionInEnv GenerateFunctionOutEnv EnvSubstituteValue GenerateStructEnv);

use vars qw($VERSION);
$VERSION = '0.01';

sub new($) {
	my ($class) = @_;
	my $self = { res => "", res_hdr => "", tabs => "", constants => {},
	             module_methods => [], module_objects => [], ready_types => [],
				 readycode => [] };
	bless($self, $class);
}

sub pidl_hdr ($$)
{
	my $self = shift;
	$self->{res_hdr} .= shift;
}

sub pidl($$)
{
	my ($self, $d) = @_;
	if ($d) {
		$self->{res} .= $self->{tabs};
		$self->{res} .= $d;
	}
	$self->{res} .= "\n";
}

sub indent($)
{
	my ($self) = @_;
	$self->{tabs} .= "\t";
}

sub deindent($)
{
	my ($self) = @_;
	$self->{tabs} = substr($self->{tabs}, 0, -1);
}

sub Import
{
	my $self = shift;
	my @imports = @_;
	foreach (@imports) {
		$_ = unmake_str($_);
		s/\.idl$//;
		$self->pidl_hdr("#include \"librpc/gen_ndr/py_$_\.h\"\n");
	}
}

sub Const($$)
{
    my ($self, $const) = @_;
	$self->register_constant($const->{NAME}, $const->{DTYPE}, $const->{VALUE});
}

sub register_constant($$$$)
{
	my ($self, $name, $type, $value) = @_;

	$self->{constants}->{$name} = [$type, $value];
}

sub EnumAndBitmapConsts($$$)
{
	my ($self, $name, $d) = @_;

	return unless (defined($d->{ELEMENTS}));

	foreach my $e (@{$d->{ELEMENTS}}) {
		$e =~ /^([A-Za-z0-9_]+)/;
		my $cname = $1;

		$self->register_constant($cname, $d, $cname);
	}
}

sub FromUnionToPythonFunction($$$$)
{
	my ($self, $mem_ctx, $type, $switch, $name) = @_;

	$self->pidl("PyObject *ret;");
	$self->pidl("");

	$self->pidl("switch ($switch) {");
	$self->indent;

	foreach my $e (@{$type->{ELEMENTS}}) {
		$self->pidl("$e->{CASE}:");

		$self->indent;

		if ($e->{NAME}) {
			$self->ConvertObjectToPython($mem_ctx, {}, $e, "$name->$e->{NAME}", "ret", "return NULL;");
		} else {
			$self->pidl("ret = Py_None;");
		}

		$self->pidl("return ret;");
		$self->pidl("");

		$self->deindent;
	}

	$self->deindent;
	$self->pidl("}");

	$self->pidl("PyErr_SetString(PyExc_TypeError, \"unknown union level\");");
	$self->pidl("return NULL;");
}

sub FromPythonToUnionFunction($$$$$)
{
	my ($self, $type, $typename, $switch, $mem_ctx, $name) = @_;

	my $has_default = 0;

	$self->pidl("$typename *ret = talloc_zero($mem_ctx, $typename);");

	$self->pidl("switch ($switch) {");
	$self->indent;

	foreach my $e (@{$type->{ELEMENTS}}) {
		$self->pidl("$e->{CASE}:");
		if ($e->{CASE} eq "default") { $has_default = 1; }
		$self->indent;
		if ($e->{NAME}) {
			$self->ConvertObjectFromPython({}, $mem_ctx, $e, $name, "ret->$e->{NAME}", "talloc_free(ret); return NULL;");
		}
		$self->pidl("break;");
		$self->deindent;
		$self->pidl("");
	}

	if (!$has_default) {
		$self->pidl("default:");
		$self->indent;
		$self->pidl("PyErr_SetString(PyExc_TypeError, \"invalid union level value\");");
		$self->pidl("talloc_free(ret);");
		$self->pidl("ret = NULL;");
		$self->deindent;
	}

	$self->deindent;
	$self->pidl("}");
	$self->pidl("");
	$self->pidl("return ret;");
}

sub PythonStruct($$$$$$)
{
	my ($self, $modulename, $prettyname, $name, $cname, $d) = @_;

	my $env = GenerateStructEnv($d, "object");

	$self->pidl("");

	my $getsetters = "NULL";

	if ($#{$d->{ELEMENTS}} > -1) {
		foreach my $e (@{$d->{ELEMENTS}}) {
			my $varname = "object->$e->{NAME}";
			$self->pidl("static PyObject *py_$name\_get_$e->{NAME}(PyObject *obj, void *closure)");
			$self->pidl("{");
			$self->indent;
			$self->pidl("$cname *object = py_talloc_get_ptr(obj);");
			$self->pidl("PyObject *py_$e->{NAME};");
			$self->ConvertObjectToPython("py_talloc_get_mem_ctx(obj)", $env, $e, $varname, "py_$e->{NAME}", "return NULL;");
			$self->pidl("return py_$e->{NAME};");
			$self->deindent;
			$self->pidl("}");
			$self->pidl("");

			$self->pidl("static int py_$name\_set_$e->{NAME}(PyObject *py_obj, PyObject *value, void *closure)");
			$self->pidl("{");
			$self->indent;
			$self->pidl("$cname *object = py_talloc_get_ptr(py_obj);");
			my $mem_ctx = "py_talloc_get_mem_ctx(py_obj)";
			my $l = $e->{LEVELS}[0];
			my $nl = GetNextLevel($e, $l);
			if ($l->{TYPE} eq "POINTER" and
				not ($nl->{TYPE} eq "ARRAY" and ($nl->{IS_FIXED} or is_charset_array($e, $nl))) and
				not ($nl->{TYPE} eq "DATA" and Parse::Pidl::Typelist::scalar_is_reference($nl->{DATA_TYPE}))) {
				$self->pidl("talloc_free($varname);");
			}
			$self->ConvertObjectFromPython($env, $mem_ctx, $e, "value", $varname, "return -1;");
			$self->pidl("return 0;");
			$self->deindent;
			$self->pidl("}");
			$self->pidl("");
		}

		$getsetters = "py_$name\_getsetters";
		$self->pidl("static PyGetSetDef ".$getsetters."[] = {");
		$self->indent;
		foreach my $e (@{$d->{ELEMENTS}}) {
			$self->pidl("{ discard_const_p(char, \"$e->{NAME}\"), py_$name\_get_$e->{NAME}, py_$name\_set_$e->{NAME} },");
		}
		$self->pidl("{ NULL }");
		$self->deindent;
		$self->pidl("};");
		$self->pidl("");
	}

	$self->pidl("static PyObject *py_$name\_new(PyTypeObject *self, PyObject *args, PyObject *kwargs)");
	$self->pidl("{");
	$self->indent;
	$self->pidl("$cname *ret = talloc_zero(NULL, $cname);");
	$self->pidl("return py_talloc_import(&$name\_Type, ret);");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");

	my $py_methods = "NULL";

	# If the struct is not public there ndr_pull/ndr_push functions will
	# be static so not callable from here
	if (has_property($d, "public")) {
		$self->pidl("static PyObject *py_$name\_ndr_pack(PyObject *py_obj)");
		$self->pidl("{");
		$self->indent;
		$self->pidl("$cname *object = py_talloc_get_ptr(py_obj);");
		$self->pidl("DATA_BLOB blob;");
		$self->pidl("enum ndr_err_code err;");
		$self->pidl("err = ndr_push_struct_blob(&blob, py_talloc_get_mem_ctx(py_obj), NULL, object, (ndr_push_flags_fn_t)ndr_push_$name);");
		$self->pidl("if (err != NDR_ERR_SUCCESS) {");
		$self->indent;
		$self->pidl("PyErr_SetNdrError(err);");
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
		$self->pidl("return PyString_FromStringAndSize((char *)blob.data, blob.length);");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");

		$self->pidl("static PyObject *py_$name\_ndr_unpack(PyObject *py_obj, PyObject *args)");
		$self->pidl("{");
		$self->indent;
		$self->pidl("$cname *object = py_talloc_get_ptr(py_obj);");
		$self->pidl("DATA_BLOB blob;");
		$self->pidl("enum ndr_err_code err;");
		$self->pidl("if (!PyArg_ParseTuple(args, \"s#:__ndr_unpack__\", &blob.data, &blob.length))");
		$self->pidl("\treturn NULL;");
		$self->pidl("");
		$self->pidl("err = ndr_pull_struct_blob_all(&blob, py_talloc_get_mem_ctx(py_obj), NULL, object, (ndr_pull_flags_fn_t)ndr_pull_$name);");
		$self->pidl("if (err != NDR_ERR_SUCCESS) {");
		$self->indent;
		$self->pidl("PyErr_SetNdrError(err);");
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
		$self->pidl("return Py_None;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
		$py_methods = "py_$name\_methods";
		$self->pidl("static PyMethodDef $py_methods\[] = {");
		$self->indent;
		$self->pidl("{ \"__ndr_pack__\", (PyCFunction)py_$name\_ndr_pack, METH_NOARGS, \"S.pack() -> blob\\nNDR pack\" },");
		$self->pidl("{ \"__ndr_unpack__\", (PyCFunction)py_$name\_ndr_unpack, METH_VARARGS, \"S.unpack(blob) -> None\\nNDR unpack\" },");
		$self->pidl("{ NULL, NULL, 0, NULL }");
		$self->deindent;
		$self->pidl("};");
		$self->pidl("");
	}

	$self->pidl_hdr("PyAPI_DATA(PyTypeObject) $name\_Type;\n");
	$self->pidl_hdr("#define $name\_Check(op) PyObject_TypeCheck(op, &$name\_Type)\n");
	$self->pidl_hdr("#define $name\_CheckExact(op) ((op)->ob_type == &$name\_Type)\n");
	$self->pidl_hdr("\n");
	my $docstring = ($self->DocString($d, $name) or "NULL");
	my $typeobject = "$name\_Type";
	$self->pidl("PyTypeObject $typeobject = {");
	$self->indent;
	$self->pidl("PyObject_HEAD_INIT(NULL) 0,");
	$self->pidl(".tp_name = \"$modulename.$prettyname\",");
	$self->pidl(".tp_basicsize = sizeof(py_talloc_Object),");
	$self->pidl(".tp_dealloc = py_talloc_dealloc,");
	$self->pidl(".tp_getset = $getsetters,");
	$self->pidl(".tp_repr = py_talloc_default_repr,");
	$self->pidl(".tp_doc = $docstring,");
	$self->pidl(".tp_methods = $py_methods,");
	$self->pidl(".tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,");
	$self->pidl(".tp_new = py_$name\_new,");
	$self->deindent;
	$self->pidl("};");

	$self->pidl("");

	return "&$typeobject";
}

sub get_metadata_var($)
{
	my ($e) = @_;
	sub get_var($) { my $x = shift; $x =~ s/\*//g; return $x; }

	 if (has_property($e, "length_is")) {
		return get_var($e->{PROPERTIES}->{length_is});
	 } elsif (has_property($e, "size_is")) {
		return get_var($e->{PROPERTIES}->{size_is});
	 }

	 return undef;
}

sub find_metadata_args($)
{
	my ($fn) = @_;
	my $metadata_args = { in => {}, out => {} };

	# Determine arguments that are metadata for other arguments (size_is/length_is)
	foreach my $e (@{$fn->{ELEMENTS}}) {
		foreach my $dir (@{$e->{DIRECTION}}) {
			 my $main = get_metadata_var($e);
			 if ($main) {
				 $metadata_args->{$dir}->{$main} = $e->{NAME};
			 }
		 }
	}

	return $metadata_args;
}

sub PythonFunctionUnpackOut($$$)
{
	my ($self, $fn, $fnname) = @_;

	my $outfnname = "unpack_$fnname\_args_out";
	my $signature = "";

	my $metadata_args = find_metadata_args($fn);

	my $env = GenerateFunctionOutEnv($fn, "r->");
	my $result_size = 0;

	$self->pidl("static PyObject *$outfnname(struct $fn->{NAME} *r)");
	$self->pidl("{");
	$self->indent;
	$self->pidl("PyObject *result = Py_None;");
	foreach my $e (@{$fn->{ELEMENTS}}) {
		next unless (grep(/out/,@{$e->{DIRECTION}}));
		next if (($metadata_args->{in}->{$e->{NAME}} and grep(/in/, @{$e->{DIRECTION}})) or
		         ($metadata_args->{out}->{$e->{NAME}}) and grep(/out/, @{$e->{DIRECTION}}));
		$self->pidl("PyObject *py_$e->{NAME};");
		$result_size++;
	}

	if ($fn->{RETURN_TYPE}) {
		$result_size++ unless ($fn->{RETURN_TYPE} eq "WERROR" or $fn->{RETURN_TYPE} eq "NTSTATUS");
	}

	my $i = 0;

	if ($result_size > 1) {
		$self->pidl("result = PyTuple_New($result_size);");
		$signature .= "(";
	} elsif ($result_size == 0) {
		$signature .= "None";
	}

	foreach my $e (@{$fn->{ELEMENTS}}) {
		next if ($metadata_args->{out}->{$e->{NAME}});
		my $py_name = "py_$e->{NAME}";
		if (grep(/out/,@{$e->{DIRECTION}})) {
			$self->ConvertObjectToPython("r", $env, $e, "r->out.$e->{NAME}", $py_name, "return NULL;");
			if ($result_size > 1) {
				$self->pidl("PyTuple_SetItem(result, $i, $py_name);");
				$i++;
				$signature .= "$e->{NAME}, ";
			} else {
				$self->pidl("result = $py_name;");
				$signature .= $e->{NAME};
			}
		}
	}

	if (defined($fn->{RETURN_TYPE}) and $fn->{RETURN_TYPE} eq "NTSTATUS") {
		$self->handle_ntstatus("r->out.result", "NULL", undef);
	} elsif (defined($fn->{RETURN_TYPE}) and $fn->{RETURN_TYPE} eq "WERROR") {
		$self->handle_werror("r->out.result", "NULL", undef);
	} elsif (defined($fn->{RETURN_TYPE})) {
		my $conv = $self->ConvertObjectToPythonData("r", $fn->{RETURN_TYPE}, "r->out.result");
		if ($result_size > 1) {
			$self->pidl("PyTuple_SetItem(result, $i, $conv);");
		} else {
			$self->pidl("result = $conv;");
		}
		$signature .= "result";
	}

	if (substr($signature, -2) eq ", ") {
		$signature = substr($signature, 0, -2);
	}
	if ($result_size > 1) {
		$signature .= ")";
	}

	$self->pidl("return result;");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");

	return ($outfnname, $signature);
}

sub PythonFunctionPackIn($$$)
{
	my ($self, $fn, $fnname) = @_;
	my $metadata_args = find_metadata_args($fn);

	my $infnname = "pack_$fnname\_args_in";

	$self->pidl("static bool $infnname(PyObject *args, PyObject *kwargs, struct $fn->{NAME} *r)");
	$self->pidl("{");
	$self->indent;
	my $args_format = "";
	my $args_string = "";
	my $args_names = "";
	my $signature = "";

	foreach my $e (@{$fn->{ELEMENTS}}) {
		next unless (grep(/in/,@{$e->{DIRECTION}}));
		next if (($metadata_args->{in}->{$e->{NAME}} and grep(/in/, @{$e->{DIRECTION}})) or
				 ($metadata_args->{out}->{$e->{NAME}}) and grep(/out/, @{$e->{DIRECTION}}));
		$self->pidl("PyObject *py_$e->{NAME};");
		$args_format .= "O";
		$args_string .= ", &py_$e->{NAME}";
		$args_names .= "\"$e->{NAME}\", ";
		$signature .= "$e->{NAME}, ";
	}
	if (substr($signature, -2) eq ", ") {
		$signature = substr($signature, 0, -2);
	}
	$self->pidl("const char *kwnames[] = {");
	$self->indent;
	$self->pidl($args_names . "NULL");
	$self->deindent;
	$self->pidl("};");

	$self->pidl("");
	$self->pidl("if (!PyArg_ParseTupleAndKeywords(args, kwargs, \"$args_format:$fn->{NAME}\", discard_const_p(char *, kwnames)$args_string)) {");
	$self->indent;
	$self->pidl("return false;");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");

	my $env = GenerateFunctionInEnv($fn, "r->");

	my $fail = "return false;";
	foreach my $e (@{$fn->{ELEMENTS}}) {
		next unless (grep(/in/,@{$e->{DIRECTION}}));
		if ($metadata_args->{in}->{$e->{NAME}}) {
			my $py_var = "py_".$metadata_args->{in}->{$e->{NAME}};
			$self->pidl("PY_CHECK_TYPE(PyList, $py_var, $fail);");
			my $val = "PyList_Size($py_var)";
			if ($e->{LEVELS}[0]->{TYPE} eq "POINTER") {
				$self->pidl("r->in.$e->{NAME} = talloc_ptrtype(r, r->in.$e->{NAME});");
				$self->pidl("*r->in.$e->{NAME} = $val;");
			} else {
				$self->pidl("r->in.$e->{NAME} = $val;");
			}
		} else {
			$self->ConvertObjectFromPython($env, "r", $e, "py_$e->{NAME}", "r->in.$e->{NAME}", $fail);
		}
	}
	$self->pidl("return true;");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");
	return ($infnname, $signature);
}

sub PythonFunction($$$)
{
	my ($self, $fn, $iface, $prettyname) = @_;

	my $fnname = "py_$fn->{NAME}";
	my $docstring = $self->DocString($fn, $fn->{NAME});

	my ($insignature, $outsignature);
	my ($infn, $outfn);

	if (has_property($fn, "todo")) {
		unless ($docstring) { $docstring = "NULL"; }
		$infn = "NULL";
		$outfn = "NULL";
	} else {
		($infn, $insignature) = $self->PythonFunctionPackIn($fn, $fnname);
		($outfn, $outsignature) = $self->PythonFunctionUnpackOut($fn, $fnname);
		my $signature = "S.$prettyname($insignature) -> $outsignature";
		if ($docstring) {
			$docstring = "\"$signature\\n\\n\"$docstring";
		} else {
			$docstring = "\"$signature\"";
		}
	}

	return ($infn, $outfn, $docstring);
}

sub handle_werror($$$$)
{
	my ($self, $var, $retval, $mem_ctx) = @_;

	$self->pidl("if (!W_ERROR_IS_OK($var)) {");
	$self->indent;
	$self->pidl("PyErr_SetWERROR($var);");
	$self->pidl("talloc_free($mem_ctx);") if ($mem_ctx);
	$self->pidl("return $retval;");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");
}

sub handle_ntstatus($$$$)
{
	my ($self, $var, $retval, $mem_ctx) = @_;

	$self->pidl("if (NT_STATUS_IS_ERR($var)) {");
	$self->indent;
	$self->pidl("PyErr_SetNTSTATUS($var);");
	$self->pidl("talloc_free($mem_ctx);") if ($mem_ctx);
	$self->pidl("return $retval;");
	$self->deindent;
	$self->pidl("}");
	$self->pidl("");
}

sub PythonType($$$$)
{
	my ($self, $modulename, $d, $interface, $basename) = @_;

	my $actual_ctype = $d;
	if ($actual_ctype->{TYPE} eq "TYPEDEF") {
		$actual_ctype = $actual_ctype->{DATA};
	}

	if ($actual_ctype->{TYPE} eq "STRUCT") {
		my $typeobject;
		my $fn_name = $d->{NAME};

		$fn_name =~ s/^$interface->{NAME}_//;
		$fn_name =~ s/^$basename\_//;


		if ($d->{TYPE} eq "STRUCT") {
			$typeobject = $self->PythonStruct($modulename, $fn_name, $d->{NAME}, mapTypeName($d), $d);
		} else {
			$typeobject = $self->PythonStruct($modulename, $fn_name, $d->{NAME}, mapTypeName($d), $d->{DATA});
		}

		$self->register_module_typeobject($fn_name, $typeobject);
	}

	if ($d->{TYPE} eq "ENUM" or $d->{TYPE} eq "BITMAP") {
		$self->EnumAndBitmapConsts($d->{NAME}, $d);
	}

	if ($d->{TYPE} eq "TYPEDEF" and ($d->{DATA}->{TYPE} eq "ENUM" or $d->{DATA}->{TYPE} eq "BITMAP")) {
		$self->EnumAndBitmapConsts($d->{NAME}, $d->{DATA});
	}

	if ($actual_ctype->{TYPE} eq "UNION" and defined($actual_ctype->{ELEMENTS})) {
		$self->pidl("PyObject *py_import_$d->{NAME}(TALLOC_CTX *mem_ctx, int level, " .mapTypeName($d) . " *in)");
		$self->pidl("{");
		$self->indent;
		$self->FromUnionToPythonFunction("mem_ctx", $actual_ctype, "level", "in") if ($actual_ctype->{TYPE} eq "UNION");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");

		$self->pidl(mapTypeName($d) . " *py_export_$d->{NAME}(TALLOC_CTX *mem_ctx, int level, PyObject *in)");
		$self->pidl("{");
		$self->indent;
		$self->FromPythonToUnionFunction($actual_ctype, mapTypeName($d), "level", "mem_ctx", "in") if ($actual_ctype->{TYPE} eq "UNION");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
	}
}

sub DocString($$$)
{
	my ($self, $d, $name) = @_;
	if (has_property($d, "helpstring")) {
		my $docstring = uc("py_doc_$name");
		$self->pidl("#define $docstring ".has_property($d, "helpstring"));
		return $docstring;
	}

	return undef;
}

sub Interface($$$)
{
	my($self,$interface,$basename) = @_;

	$self->pidl_hdr("#ifndef _HEADER_PYTHON_$interface->{NAME}\n");
	$self->pidl_hdr("#define _HEADER_PYTHON_$interface->{NAME}\n\n");

	$self->pidl_hdr("\n");

	$self->Const($_) foreach (@{$interface->{CONSTS}});

	foreach my $d (@{$interface->{TYPES}}) {
		next if has_property($d, "nopython");

		$self->PythonType($basename, $d, $interface, $basename);
	}

	if (defined $interface->{PROPERTIES}->{uuid}) {
		$self->pidl_hdr("PyAPI_DATA(PyTypeObject) $interface->{NAME}_InterfaceType;\n");
		$self->pidl("");

		my @fns = ();

		foreach my $d (@{$interface->{FUNCTIONS}}) {
			next if not defined($d->{OPNUM});
			next if has_property($d, "nopython");

			my $prettyname = $d->{NAME};

			$prettyname =~ s/^$interface->{NAME}_//;
			$prettyname =~ s/^$basename\_//;

			my ($infn, $outfn, $fndocstring) = $self->PythonFunction($d, $interface->{NAME}, $prettyname);

			push (@fns, [$infn, $outfn, "dcerpc_$d->{NAME}", $prettyname, $fndocstring, $d->{OPNUM}]);
		}

		$self->pidl("const struct PyNdrRpcMethodDef py_ndr_$interface->{NAME}\_methods[] = {");
		$self->pidl_hdr("extern const struct PyNdrRpcMethodDef py_ndr_$interface->{NAME}\_methods[];");
		$self->indent;
		foreach my $d (@fns) {
			my ($infn, $outfn, $callfn, $prettyname, $docstring, $opnum) = @$d;
			$self->pidl("{ \"$prettyname\", $docstring, (dcerpc_call_fn)$callfn, (py_data_pack_fn)$infn, (py_data_unpack_fn)$outfn, $opnum, &ndr_table_$interface->{NAME} },");
		}
		$self->pidl("{ NULL }");
		$self->deindent;
		$self->pidl("};");
		$self->pidl("");

		$self->pidl("static PyObject *interface_$interface->{NAME}_new(PyTypeObject *self, PyObject *args, PyObject *kwargs)");
		$self->pidl("{");
		$self->indent;
		$self->pidl("dcerpc_InterfaceObject *ret;");
		$self->pidl("const char *binding_string;");
		$self->pidl("struct cli_credentials *credentials;");
		$self->pidl("struct loadparm_context *lp_ctx = NULL;");
		$self->pidl("PyObject *py_lp_ctx = Py_None, *py_credentials = Py_None, *py_basis = Py_None;");
		$self->pidl("TALLOC_CTX *mem_ctx = NULL;");
		$self->pidl("struct event_context *event_ctx;");
		$self->pidl("NTSTATUS status;");
		$self->pidl("");
		$self->pidl("const char *kwnames[] = {");
		$self->indent;
		$self->pidl("\"binding\", \"lp_ctx\", \"credentials\", \"basis_connection\", NULL");
		$self->deindent;
		$self->pidl("};");
		$self->pidl("extern struct loadparm_context *lp_from_py_object(PyObject *py_obj);");
		$self->pidl("extern struct cli_credentials *cli_credentials_from_py_object(PyObject *py_obj);");
		$self->pidl("");
		$self->pidl("if (!PyArg_ParseTupleAndKeywords(args, kwargs, \"s|OOO:$interface->{NAME}\", discard_const_p(char *, kwnames), &binding_string, &py_lp_ctx, &py_credentials, &py_basis)) {");
		$self->indent;
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
		$self->pidl("lp_ctx = lp_from_py_object(py_lp_ctx);");
		$self->pidl("if (lp_ctx == NULL) {");
		$self->indent;
		$self->pidl("PyErr_SetString(PyExc_TypeError, \"Expected loadparm context\");");
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");

		$self->pidl("credentials = cli_credentials_from_py_object(py_credentials);");
		$self->pidl("if (credentials == NULL) {");
		$self->indent;
		$self->pidl("PyErr_SetString(PyExc_TypeError, \"Expected credentials\");");
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");

		$self->pidl("ret = PyObject_New(dcerpc_InterfaceObject, &$interface->{NAME}_InterfaceType);");
		$self->pidl("");
		$self->pidl("event_ctx = event_context_init(mem_ctx);");
		$self->pidl("");

		$self->pidl("if (py_basis != Py_None) {");
		$self->indent;
		$self->pidl("struct dcerpc_pipe *base_pipe;");
		$self->pidl("");
		$self->pidl("if (!PyObject_TypeCheck(py_basis, &dcerpc_InterfaceType)) {");
		$self->indent;
		$self->pidl("PyErr_SetString(PyExc_ValueError, \"basis_connection must be a DCE/RPC connection\");");
		$self->pidl("talloc_free(mem_ctx);");
		$self->pidl("return NULL;");
		$self->deindent;
		$self->pidl("}");
		$self->pidl("");
		$self->pidl("base_pipe = ((dcerpc_InterfaceObject *)py_basis)->pipe;");
		$self->pidl("");
		$self->pidl("status = dcerpc_secondary_context(base_pipe, &ret->pipe, &ndr_table_$interface->{NAME});");
		$self->deindent;
		$self->pidl("} else {");
		$self->indent;
		$self->pidl("status = dcerpc_pipe_connect(NULL, &ret->pipe, binding_string, ");
		$self->pidl("             &ndr_table_$interface->{NAME}, credentials, event_ctx, lp_ctx);");
		$self->deindent;
		$self->pidl("}");
		$self->handle_ntstatus("status", "NULL", "mem_ctx");

		$self->pidl("ret->pipe->conn->flags |= DCERPC_NDR_REF_ALLOC;");

		$self->pidl("return (PyObject *)ret;");
		$self->deindent;
		$self->pidl("}");

		$self->pidl("");

		my $signature =
"\"$interface->{NAME}(binding, lp_ctx=None, credentials=None) -> connection\\n\"
\"\\n\"
\"binding should be a DCE/RPC binding string (for example: ncacn_ip_tcp:127.0.0.1)\\n\"
\"lp_ctx should be a path to a smb.conf file or a param.LoadParm object\\n\"
\"credentials should be a credentials.Credentials object.\\n\\n\"";

		my $docstring = $self->DocString($interface, $interface->{NAME});

		if ($docstring) {
			$docstring = "$signature$docstring";
		} else {
			$docstring = $signature;
		}

		$self->pidl("PyTypeObject $interface->{NAME}_InterfaceType = {");
		$self->indent;
		$self->pidl("PyObject_HEAD_INIT(NULL) 0,");
		$self->pidl(".tp_name = \"$basename.$interface->{NAME}\",");
		$self->pidl(".tp_basicsize = sizeof(dcerpc_InterfaceObject),");
		$self->pidl(".tp_base = &dcerpc_InterfaceType,");
		$self->pidl(".tp_doc = $docstring,");
		$self->pidl(".tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,");
		$self->pidl(".tp_new = interface_$interface->{NAME}_new,");
		$self->deindent;
		$self->pidl("};");

		$self->pidl("");

		$self->register_module_typeobject($interface->{NAME}, "&$interface->{NAME}_InterfaceType");
		$self->register_module_readycode(["if (!PyInterface_AddNdrRpcMethods(&$interface->{NAME}_InterfaceType, py_ndr_$interface->{NAME}\_methods))", "\treturn;", ""]);
	}

	$self->pidl_hdr("\n");
	$self->pidl_hdr("#endif /* _HEADER_NDR_$interface->{NAME} */\n");
}

sub register_module_method($$$$$)
{
	my ($self, $fn_name, $pyfn_name, $flags, $doc) = @_;

	push (@{$self->{module_methods}}, [$fn_name, $pyfn_name, $flags, $doc])
}

sub register_module_typeobject($$$)
{
	my ($self, $name, $py_name) = @_;

	$self->register_module_object($name, "(PyObject *)$py_name");

	$self->check_ready_type($py_name);
}

sub check_ready_type($$)
{
	my ($self, $py_name) = @_;
	push (@{$self->{ready_types}}, $py_name) unless (grep(/^$py_name$/,@{$self->{ready_types}}));
}

sub register_module_readycode($$)
{
	my ($self, $code) = @_;

	push (@{$self->{readycode}}, @$code);
}

sub register_module_object($$$)
{
	my ($self, $name, $py_name) = @_;

	push (@{$self->{module_objects}}, [$name, $py_name])
}

sub assign($$$)
{
	my ($self, $dest, $src) = @_;
	if ($dest =~ /^\&/) {
		$self->pidl("memcpy($dest, $src, sizeof(" . get_value_of($dest) . "));");
	} else {
		$self->pidl("$dest = $src;");
	}
}

sub ConvertObjectFromPythonData($$$$$$)
{
	my ($self, $mem_ctx, $cvar, $ctype, $target, $fail) = @_;

	die("undef type for $cvar") unless(defined($ctype));

	$ctype = resolveType($ctype);

	my $actual_ctype = $ctype;
	if ($ctype->{TYPE} eq "TYPEDEF") {
		$actual_ctype = $ctype->{DATA};
	}

	if ($actual_ctype->{TYPE} eq "ENUM" or $actual_ctype->{TYPE} eq "BITMAP" or
		$actual_ctype->{TYPE} eq "SCALAR" and (
		expandAlias($actual_ctype->{NAME}) =~ /^(u?int[0-9]*|hyper|NTTIME|time_t|NTTIME_hyper|NTTIME_1sec|dlong|udlong|udlongr)$/)) {
		$self->pidl("PY_CHECK_TYPE(PyInt, $cvar, $fail);");
		$self->pidl("$target = PyInt_AsLong($cvar);");
		return;
	}

	if ($actual_ctype->{TYPE} eq "STRUCT" or $actual_ctype->{TYPE} eq "INTERFACE") {
		$self->pidl("PY_CHECK_TYPE($ctype->{NAME}, $cvar, $fail);");
		$self->assign($target, "py_talloc_get_ptr($cvar)");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "DATA_BLOB") {
		$self->pidl("$target = data_blob_talloc($mem_ctx, PyString_AsString($cvar), PyString_Size($cvar));");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and
		($actual_ctype->{NAME} eq "string" or $actual_ctype->{NAME} eq "nbt_string" or $actual_ctype->{NAME} eq "nbt_name" or $actual_ctype->{NAME} eq "wrepl_nbt_name")) {
		$self->pidl("$target = talloc_strdup($mem_ctx, PyString_AsString($cvar));");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "ipv4address") {
		$self->pidl("$target = PyString_AsString($cvar);");
		return;
		}


	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "NTSTATUS") {
		$self->pidl("$target = NT_STATUS(PyInt_AsLong($cvar));");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "WERROR") {
		$self->pidl("$target = W_ERROR(PyInt_AsLong($cvar));");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "string_array") {
		$self->pidl("$target = PyCObject_AsVoidPtr($cvar);");
		return;
	}

	if ($actual_ctype->{TYPE} eq "SCALAR" and $actual_ctype->{NAME} eq "pointer") {
		$self->assign($target, "PyCObject_AsVoidPtr($cvar)");
		return;
	}

	fatal($ctype, "unknown type $actual_ctype->{TYPE} for ".mapTypeName($ctype) . ": $cvar");

}

sub ConvertObjectFromPythonLevel($$$$$$$$)
{
	my ($self, $env, $mem_ctx, $py_var, $e, $l, $var_name, $fail) = @_;
	my $nl = GetNextLevel($e, $l);

	if ($l->{TYPE} eq "POINTER") {
		if ($nl->{TYPE} eq "DATA" and Parse::Pidl::Typelist::scalar_is_reference($nl->{DATA_TYPE})) {
			$self->ConvertObjectFromPythonLevel($env, $mem_ctx, $py_var, $e, $nl, $var_name, $fail);
			return;
		}
		if ($l->{POINTER_TYPE} ne "ref") {
			$self->pidl("if ($py_var == Py_None) {");
			$self->indent;
			$self->pidl("$var_name = NULL;");
			$self->deindent;
			$self->pidl("} else {");
			$self->indent;
		}
		$self->pidl("$var_name = talloc_ptrtype($mem_ctx, $var_name);");
		$self->ConvertObjectFromPythonLevel($env, $mem_ctx, $py_var, $e, $nl, get_value_of($var_name), $fail);
		if ($l->{POINTER_TYPE} ne "ref") {
			$self->deindent;
			$self->pidl("}");
		}
	} elsif ($l->{TYPE} eq "ARRAY") {
		my $pl = GetPrevLevel($e, $l);
		if ($pl && $pl->{TYPE} eq "POINTER") {
			$var_name = get_pointer_to($var_name);
		}

		if (is_charset_array($e, $l)) {
			$self->pidl("PY_CHECK_TYPE(PyUnicode, $py_var, $fail);");
			# FIXME: Use Unix charset setting rather than utf-8
			$self->pidl($var_name . " = PyString_AsString(PyUnicode_AsEncodedString($py_var, \"utf-8\", \"ignore\"));");
		} else {
			my $counter = "$e->{NAME}_cntr_$l->{LEVEL_INDEX}";
			$self->pidl("PY_CHECK_TYPE(PyList, $py_var, $fail);");
			$self->pidl("{");
			$self->indent;
			$self->pidl("int $counter;");
			if (ArrayDynamicallyAllocated($e, $l)) {
				$self->pidl("$var_name = talloc_array_ptrtype($mem_ctx, $var_name, PyList_Size($py_var));");
			}
			$self->pidl("for ($counter = 0; $counter < PyList_Size($py_var); $counter++) {");
			$self->indent;
			$self->ConvertObjectFromPythonLevel($env, $var_name, "PyList_GetItem($py_var, $counter)", $e, GetNextLevel($e, $l), $var_name."[$counter]", $fail);
			$self->deindent;
			$self->pidl("}");
			$self->deindent;
			$self->pidl("}");
		}
	} elsif ($l->{TYPE} eq "DATA") {

		if (not Parse::Pidl::Typelist::is_scalar($l->{DATA_TYPE})) {
			$var_name = get_pointer_to($var_name);
		}
		$self->ConvertObjectFromPythonData($mem_ctx, $py_var, $l->{DATA_TYPE}, $var_name, $fail);
	} elsif ($l->{TYPE} eq "SWITCH") {
		$var_name = get_pointer_to($var_name);
		my $switch = ParseExpr($l->{SWITCH_IS}, $env, $e);
		$self->assign($var_name, "py_export_" . GetNextLevel($e, $l)->{DATA_TYPE} . "($mem_ctx, $switch, $py_var)");
	} elsif ($l->{TYPE} eq "SUBCONTEXT") {
		$self->ConvertObjectFromPythonLevel($env, $mem_ctx, $py_var, $e, GetNextLevel($e, $l), $var_name, $fail);
	} else {
		die("unknown level type $l->{TYPE}");
	}
}

sub ConvertObjectFromPython($$$$$$$)
{
	my ($self, $env, $mem_ctx, $ctype, $cvar, $target, $fail) = @_;

	$self->ConvertObjectFromPythonLevel($env, $mem_ctx, $cvar, $ctype, $ctype->{LEVELS}[0], $target, $fail);
}

sub ConvertScalarToPython($$$)
{
	my ($self, $ctypename, $cvar) = @_;

	die("expected string for $cvar, not $ctypename") if (ref($ctypename) eq "HASH");

	$ctypename = expandAlias($ctypename);

	if ($ctypename =~ /^(char|u?int[0-9]*|hyper|dlong|udlong|udlongr|time_t|NTTIME_hyper|NTTIME|NTTIME_1sec)$/) {
		return "PyInt_FromLong($cvar)";
	}

	if ($ctypename eq "DATA_BLOB") {
		return "PyString_FromStringAndSize((char *)($cvar).data, ($cvar).length)";
	}

	if ($ctypename eq "NTSTATUS") {
		return "PyErr_FromNTSTATUS($cvar)";
	}

	if ($ctypename eq "WERROR") {
		return "PyErr_FromWERROR($cvar)";
	}

	if (($ctypename eq "string" or $ctypename eq "nbt_string" or $ctypename eq "nbt_name" or $ctypename eq "wrepl_nbt_name")) {
		return "PyString_FromString($cvar)";
	}

	# Not yet supported
	if ($ctypename eq "string_array") { return "PyCObject_FromVoidPtr($cvar)"; }
	if ($ctypename eq "ipv4address") { return "PyString_FromString($cvar)"; }
	if ($ctypename eq "pointer") {
		return "PyCObject_FromVoidPtr($cvar, talloc_free)";
	}

	die("Unknown scalar type $ctypename");
}

sub ConvertObjectToPythonData($$$$$)
{
	my ($self, $mem_ctx, $ctype, $cvar) = @_;

	die("undef type for $cvar") unless(defined($ctype));

	$ctype = resolveType($ctype);

	my $actual_ctype = $ctype;
	if ($ctype->{TYPE} eq "TYPEDEF") {
		$actual_ctype = $ctype->{DATA};
	}

	if ($actual_ctype->{TYPE} eq "ENUM") {
		return $self->ConvertScalarToPython(Parse::Pidl::Typelist::enum_type_fn($actual_ctype), $cvar);
	} elsif ($actual_ctype->{TYPE} eq "BITMAP") {
		return $self->ConvertScalarToPython(Parse::Pidl::Typelist::bitmap_type_fn($actual_ctype), $cvar);
	} elsif ($actual_ctype->{TYPE} eq "SCALAR") {
		return $self->ConvertScalarToPython($actual_ctype->{NAME}, $cvar);
	} elsif ($actual_ctype->{TYPE} eq "UNION") {
		fatal($ctype, "union without discriminant: " . mapTypeName($ctype) . ": $cvar");
	} elsif ($actual_ctype->{TYPE} eq "STRUCT" or $actual_ctype->{TYPE} eq "INTERFACE") {
		return "py_talloc_import_ex(&$ctype->{NAME}_Type, $mem_ctx, $cvar)";
	}

	fatal($ctype, "unknown type $actual_ctype->{TYPE} for ".mapTypeName($ctype) . ": $cvar");
}

sub fail_on_null($$$)
{
	my ($self, $var, $fail) = @_;
	$self->pidl("if ($var == NULL) {");
	$self->indent;
	$self->pidl($fail);
	$self->deindent;
	$self->pidl("}");
}

sub ConvertObjectToPythonLevel($$$$$$)
{
	my ($self, $mem_ctx, $env, $e, $l, $var_name, $py_var, $fail) = @_;
	my $nl = GetNextLevel($e, $l);

	if ($l->{TYPE} eq "POINTER") {
		if ($nl->{TYPE} eq "DATA" and Parse::Pidl::Typelist::scalar_is_reference($nl->{DATA_TYPE})) {
			$self->ConvertObjectToPythonLevel($var_name, $env, $e, $nl, $var_name, $py_var, $fail);
			return;
		}
		if ($l->{POINTER_TYPE} ne "ref") {
			$self->pidl("if ($var_name == NULL) {");
			$self->indent;
			$self->pidl("$py_var = Py_None;");
			$self->deindent;
			$self->pidl("} else {");
			$self->indent;
		}
		$self->ConvertObjectToPythonLevel($var_name, $env, $e, $nl, get_value_of($var_name), $py_var, $fail);
		if ($l->{POINTER_TYPE} ne "ref") {
			$self->deindent;
			$self->pidl("}");
		}
	} elsif ($l->{TYPE} eq "ARRAY") {
		my $pl = GetPrevLevel($e, $l);
		if ($pl && $pl->{TYPE} eq "POINTER") {
			$var_name = get_pointer_to($var_name);
		}

		if (is_charset_array($e, $l)) {
			# FIXME: Use Unix charset setting rather than utf-8
			$self->pidl("$py_var = PyUnicode_Decode($var_name, strlen($var_name), \"utf-8\", \"ignore\");");
		} else {
			die("No SIZE_IS for array $var_name") unless (defined($l->{SIZE_IS}));
			my $length = $l->{SIZE_IS};
			if (defined($l->{LENGTH_IS})) {
				$length = $l->{LENGTH_IS};
			}

			$length = ParseExpr($length, $env, $e);
			$self->pidl("$py_var = PyList_New($length);");
			$self->fail_on_null($py_var, $fail);
			$self->pidl("{");
			$self->indent;
			my $counter = "$e->{NAME}_cntr_$l->{LEVEL_INDEX}";
			$self->pidl("int $counter;");
			$self->pidl("for ($counter = 0; $counter < $length; $counter++) {");
			$self->indent;
			my $member_var = "py_$e->{NAME}_$l->{LEVEL_INDEX}";
			$self->pidl("PyObject *$member_var;");
			$self->ConvertObjectToPythonLevel($var_name, $env, $e, GetNextLevel($e, $l), $var_name."[$counter]", $member_var, $fail);
			$self->pidl("PyList_SetItem($py_var, $counter, $member_var);");
			$self->deindent;
			$self->pidl("}");
			$self->deindent;
			$self->pidl("}");
		}
	} elsif ($l->{TYPE} eq "SWITCH") {
		$var_name = get_pointer_to($var_name);
		my $switch = ParseExpr($l->{SWITCH_IS}, $env, $e);
		$self->pidl("$py_var = py_import_" . GetNextLevel($e, $l)->{DATA_TYPE} . "($mem_ctx, $switch, $var_name);");
		$self->fail_on_null($py_var, $fail);

	} elsif ($l->{TYPE} eq "DATA") {
		if (not Parse::Pidl::Typelist::is_scalar($l->{DATA_TYPE})) {
			$var_name = get_pointer_to($var_name);
		}
		my $conv = $self->ConvertObjectToPythonData($mem_ctx, $l->{DATA_TYPE}, $var_name);
		$self->pidl("$py_var = $conv;");
	} elsif ($l->{TYPE} eq "SUBCONTEXT") {
		$self->ConvertObjectToPythonLevel($mem_ctx, $env, $e, GetNextLevel($e, $l), $var_name, $py_var, $fail);
	} else {
		die("Unknown level type $l->{TYPE} $var_name");
	}
}

sub ConvertObjectToPython($$$$$$)
{
	my ($self, $mem_ctx, $env, $ctype, $cvar, $py_var, $fail) = @_;

	$self->ConvertObjectToPythonLevel($mem_ctx, $env, $ctype, $ctype->{LEVELS}[0], $cvar, $py_var, $fail);
}

sub Parse($$$$$)
{
    my($self,$basename,$ndr,$ndr_hdr,$hdr) = @_;

    my $py_hdr = $hdr;
    $py_hdr =~ s/ndr_([^\/]+)$/py_$1/g;

    $self->pidl_hdr("/* header auto-generated by pidl */\n\n");

    $self->pidl("
/* Python wrapper functions auto-generated by pidl */
#include \"includes.h\"
#include <Python.h>
#include \"librpc/rpc/dcerpc.h\"
#include \"scripting/python/pytalloc.h\"
#include \"librpc/rpc/pyrpc.h\"
#include \"lib/events/events.h\"
#include \"$hdr\"
#include \"$ndr_hdr\"
#include \"$py_hdr\"

");

	foreach my $x (@$ndr) {
		($x->{TYPE} eq "IMPORT") && $self->Import(@{$x->{PATHS}});
	    ($x->{TYPE} eq "INTERFACE") && $self->Interface($x, $basename);
	}

	$self->pidl("static PyMethodDef $basename\_methods[] = {");
	$self->indent;
	foreach (@{$self->{module_methods}}) {
		my ($fn_name, $pyfn_name, $flags, $doc) = @$_;
		$self->pidl("{ \"$fn_name\", (PyCFunction)$pyfn_name, $flags, $doc },");
	}

	$self->pidl("{ NULL, NULL, 0, NULL }");
	$self->deindent;
	$self->pidl("};");

	$self->pidl("");

	$self->pidl("void init$basename(void)");
	$self->pidl("{");
	$self->indent;
	$self->pidl("PyObject *m;");
	$self->pidl("");

	foreach (@{$self->{ready_types}}) {
		$self->pidl("if (PyType_Ready($_) < 0)");
		$self->pidl("\treturn;");
	}

	$self->pidl($_) foreach (@{$self->{readycode}});

	$self->pidl("");

	$self->pidl("m = Py_InitModule3(\"$basename\", $basename\_methods, \"$basename DCE/RPC\");");
	$self->pidl("if (m == NULL)");
	$self->pidl("\treturn;");
	$self->pidl("");
	foreach my $name (keys %{$self->{constants}}) {
		my $py_obj;
		my ($ctype, $cvar) = @{$self->{constants}->{$name}};
		if ($cvar =~ /^[0-9]+$/ or $cvar =~ /^0x[0-9a-fA-F]+$/) {
			$py_obj = "PyInt_FromLong($cvar)";
		} elsif ($cvar =~ /^".*"$/) {
			$py_obj = "PyString_FromString($cvar)";
		} else {
			$py_obj = $self->ConvertObjectToPythonData("NULL", expandAlias($ctype), $cvar);
		}

		$self->pidl("PyModule_AddObject(m, \"$name\", $py_obj);");
	}

	foreach (@{$self->{module_objects}}) {
		my ($object_name, $c_name) = @$_;
		$self->pidl("Py_INCREF($c_name);");
		$self->pidl("PyModule_AddObject(m, \"$object_name\", $c_name);");
	}

	$self->deindent;
	$self->pidl("}");
    return ($self->{res_hdr}, $self->{res});
}

1;
