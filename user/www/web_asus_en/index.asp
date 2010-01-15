<html>
<head>
<title>ZVMODELVZ</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Expires" content="Fri, Jun 12 1981 00:00:00 GMT">
<meta http-equiv="Cache-control" content="no-cache,must-revalidate">
<meta http-equiv="Pragma" content="no-cache">
</HEAD>
<script language="JavaScript">
var Add_Flag = "";
var Edit_Flag = "";
var Mkdir_Flag = "";
var Share_Path = "";
//each node in the tree is an Array with 4+n positions 
//  node[0] is 0/1 when the node is closed/open
//  node[1] is 0/1 when the folder is closed/open
//  node[2] is 1 if the children of the node are documents 
//  node[3] is the name of the folder
//  node[4]...node[4+n] are the n children nodes

// ***************
// Building the data in the tree
function isMode()
{   
   if (parent.titleFrame.document.form.wan_nat_x.value == '1')
      mode = 'Gateway';
   else if (parent.titleFrame.document.form.wan_route_x.value == 'IP_Routed')
      mode = 'Router';   
   else
      mode = 'AP';
 
   return mode;
}

function isModel()
{
   model = 'WL600';
   pid = parent.titleFrame.document.form.productid.value;
   
   if (pid.indexOf("WL520gc")!=-1 ||
   	    pid.indexOf("WL500g-X")!=-1 ||	
   	    pid.indexOf("WL500g-E")!=-1 ||
   	    pid.indexOf("WL550gE")!=-1 ||	
            pid.indexOf("WL530")!=-1) model = 'WL520gc';
   else if (pid.indexOf("WL500")!=-1 || 
		pid.indexOf("WL700")!=-1 || 
		pid.indexOf("WL500gpv2")!=-1 ||
		pid.indexOf("WLMIMO")!=-1) model = 'WL500';
   else if (pid.indexOf("WL331")!=-1) model = 'WL331';   
   else if (pid.indexOf("SnapAP")!=-1) model = 'SnapAP';
   else if (pid.indexOf("WL300")!=-1) model = 'WL300';
   else if (pid.indexOf("WL550")!=-1) model = 'WL550';
   else if (pid.indexOf("RT-N15")!=-1) model = 'RT-N15';
         
   return model;
}

function isModel2()
{
   model2 = 'WL600';
   pid = parent.titleFrame.document.form.productid.value;
   if (pid.indexOf("WL530")!=-1) model2 = 'WL530';        
   else if (pid.indexOf("WL500g-X")!=-1 || pid.indexOf("WL500g-E")!=-1 || pid.indexOf("WL520")!=-1 || pid.indexOf("WL550gE")!=-1) model2 = 'WL520';     
   return model2;
}

function isModel3()
{
   model3 = 'WL500';
   pid = parent.titleFrame.document.form.productid.value;
         
   if (pid.indexOf("WL550gE")!=-1) model3 = 'WL550gE';
   
   return model3;
}

function isBand()
{
   band = 'g';
   pid = parent.titleFrame.document.form.productid.value;
   
   if (pid.indexOf("WL500b")!=-1) band = 'b';   
         
   return band;
}

function isCard()
{
   card = 'broadcom';
   pid = parent.titleFrame.document.form.productid.value;   
   if (pid.indexOf("WL500bv2")!=-1) card = 'ralink';         
   return card;
}

function isFlash()
{
   flash = '4MB';
   
   pid = parent.titleFrame.document.form.productid.value;
   if (pid.indexOf("WL331g")!=-1) flash = '2MB';        
   else if (pid.indexOf("WL500g.Lite")!=-1) flash = '2MB';         
   else if (pid.indexOf("WL500b.Lite")!=-1) flash = '2MB';          
   else if (pid.indexOf("WL500g-X")!=-1 || pid.indexOf("WL500g-E")!=-1 || pid.indexOf("WL520gc")!=-1 || pid.indexOf("WL550gE")!=-1) flash = '2MB';
   return flash;
}

function supportCDMA()
{  
   return parent.titleFrame.document.form.support_cdma.value;
}

function logout()
{
   if (confirm('<#JS_logout#>'))
   {
//   	location="Logout.asp";
   	logout_flag=1;
   }   
}


function generateTree()
    {
    var aux1, aux2, aux3, aux4;

    mode = isMode();

    foldersTree = folderNode(mode + 'Setting');
    
    aux1 = appendChild(foldersTree, leafNode("<#t1Home#>"));
    aux1 = appendChild(foldersTree, leafNode("<#t1Quick#>"));
    if (isModel() == 'WL550')    
    {      
          appendChild(aux1, generateDocEntry(0, "t2QWidzard", "Advanced_Wireless_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "t2QNetwork", "Advanced_Wireless_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "t2QDisk", "Advanced_UniqueKey_Content.asp", "")); 
    }                
    
    aux1 = appendChild(foldersTree, leafNode("<#t1Wireless#>"));
    if (isModel() == 'WL600')    
    {      
          appendChild(aux1, generateDocEntry(0, "<#t2IF#>", "Advanced_Wireless_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2AES#>", "Advanced_UniqueKey_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2ACL#>", "Advanced_ACL_Content.asp",  ""));
    }
    else if(isModel() == 'WL520gc')
    {
          appendChild(aux1, generateDocEntry(0, "<#t2IF#>", "Advanced_Wireless_Content.asp", ""));
          //appendChild(aux1, generateDocEntry(0, "Certificate", "Advanced_WCertificate_Content.asp", ""))          
          appendChild(aux1, generateDocEntry(0, "<#t2BR#>", "Advanced_WMode_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2ACL#>", "Advanced_ACL_Content.asp",  ""));
          appendChild(aux1, generateDocEntry(0, "<#t2RADIUS#>", "Advanced_WSecurity_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2Advanced#>", "Advanced_WAdvanced_Content.asp", ""));
          
          if (isModel3() == 'WL550gE' && mode== 'Gateway')
     	  {     	  
           	appendChild(aux1, generateDocEntry(0, "<#t2Guest#>", "Advanced_WirelessGuest_Content.asp", ""));
     	  }         
    }
    else if(isModel() == 'SnapAP')
    {
          appendChild(aux1, generateDocEntry(0, "<#t2IF#>", "Advanced_Wireless_Content.asp", ""));
          //appendChild(aux1, generateDocEntry(0, "Certificate", "Advanced_WCertificate_Content.asp", ""))
          //appendChild(aux1, generateDocEntry(0, "<#t2BR#>", "Advanced_WMode_Content.asp", ""))              
          appendChild(aux1, generateDocEntry(0, "<#t2ACL#>", "Advanced_ACL_Content.asp",  ""));
          //appendChild(aux1, generateDocEntry(0, "<#t2RADIUS#>", "Advanced_WSecurity_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2Advanced#>", "Advanced_WAdvanced_Content.asp", ""));
    }   
    else if (window.top.isCard() == 'ralink')
    {
          appendChild(aux1, generateDocEntry(0, "<#t2IF#>", "Advanced_Wireless_Content.asp", ""));
          //appendChild(aux1, generateDocEntry(0, "Certificate", "Advanced_WCertificate_Content.asp", ""))
          appendChild(aux1, generateDocEntry(0, "<#t2BR#>", "Advanced_WMode_Content.asp", ""));             
          appendChild(aux1, generateDocEntry(0, "<#t2ACL#>", "Advanced_ACL_Content.asp",  ""));
          appendChild(aux1, generateDocEntry(0, "<#t2Advanced#>", "Advanced_WAdvanced_Content.asp", ""));
    }
    else
    {
          appendChild(aux1, generateDocEntry(0, "<#t2IF#>", "Advanced_Wireless_Content.asp", ""));
          //appendChild(aux1, generateDocEntry(0, "Certificate", "Advanced_WCertificate_Content.asp", ""))
          appendChild(aux1, generateDocEntry(0, "<#t2BR#>", "Advanced_WMode_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2ACL#>", "Advanced_ACL_Content.asp",  ""));
          appendChild(aux1, generateDocEntry(0, "<#t2RADIUS#>", "Advanced_WSecurity_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2Advanced#>", "Advanced_WAdvanced_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "<#t2WPS#>", "Advanced_WWPS_Content.asp", ""));
          //if (mode != 'AP')
          //	appendChild(aux1, generateDocEntry(0, "<#t2Guest#>", "Advanced_WirelessGuest_Content.asp", ""))
    }   
    
    if (mode == 'AP')
    {   
    	   aux1 = appendChild(foldersTree, leafNode("<#t1IP#>"));
           appendChild(aux1, generateDocEntry(0, "<#t2LAN#>", "Advanced_APLAN_Content.asp", ""));
    }       
    else 
    {  
    	   aux1 = appendChild(foldersTree, leafNode("<#t1IP#>"));
           appendChild(aux1, generateDocEntry(0, "<#t2WANLAN#>", "Advanced_LANWAN_Content.asp", ""));
           appendChild(aux1, generateDocEntry(0, "<#t2DHCP#>", "Advanced_DHCP_Content.asp", ""));
           
           if (isModel()!='SnapAP')    
           {
                      appendChild(aux1, generateDocEntry(0, "<#t2SRoute#>", "Advanced_GWStaticRoute_Content.asp", ""));
           }

           if(isModel() == 'RT-N15') //2007.06.12 Yau add for ASUSDDNS function
           {
                appendChild(aux1, generateDocEntry(0, "<#t2Misc#>", "Advanced_Other_Content.asp", ""));
                appendChild(aux1, generateDocEntry(0, "DDNS", "Advanced_ASUSDDNS_Content.asp", ""));
           }
           else //End of Yau add
	        appendChild(aux1, generateDocEntry(0, "<#t2Misc#>", "Advanced_DDNS_Content.asp", ""));
           
	   if (mode == 'Gateway')
	   {
              aux1 = appendChild(foldersTree, leafNode("<#t1NAT#>"));
    	      //if (isModel() == 'WL300')                             
    	      //	appendChild(aux1, generateDocEntry(0, "<#t2Trig#>",  "Advanced_PortMapping_Content.asp", ""))                  
    	      //else	    	       
               	      
    	      appendChild(aux1, generateDocEntry(0, "<#t2Trig#>",  "Advanced_PortTrigger_Content.asp", ""));   	      
              appendChild(aux1, generateDocEntry(0, "<#t2VS#>",  "Advanced_VirtualServer_Content.asp", ""));
              appendChild(aux1, generateDocEntry(0, "<#t2VDMZ#>",  "Advanced_Exposed_Content.asp", ""));
           }
           if (isModel()!='SnapAP')     
           {
    	      aux1 = appendChild(foldersTree, leafNode("<#t1IFW#>"))    	
    	      appendChild(aux1, generateDocEntry(0, "<#t2FilterBasic#>",  "Advanced_BasicFirewall_Content.asp", ""));
              appendChild(aux1, generateDocEntry(0, "<#t2WLFilter#>",  "Advanced_Firewall_Content.asp", ""));
              appendChild(aux1, generateDocEntry(0, "<#t2URLFilter#>",  "Advanced_URLFilter_Content.asp", ""));
              //appendChild(aux1, generateDocEntry(0, "WAN & ZVMODELVZ Filter",  "Advanced_LFirewall_Content.asp", ""))      
           }
           
    	   //if (isModel() != 'WL300' && isModel() != 'WL520' && isModel() != 'SnapAP')    
    	   //{          
    	   //   aux1 = appendChild(foldersTree, leafNode("<#t1WFW#>"))
    	   //       appendChild(aux1, generateDocEntry(0, "<#t2WConfig#>",  "Advanced_DMZIP_Content.asp", ""))                           
    	   //       appendChild(aux1, generateDocEntry(0, "<#t2WDHCP#>", "Advanced_DMZDHCP_Content.asp", ""))
    	   //	   appendChild(aux1, generateDocEntry(0, "<#t2WWANFilter#>",  "Advanced_DMZDWFilter_Content.asp", ""))                                                                                          
    	   //       appendChild(aux1, generateDocEntry(0, "<#t2WLANFilter#>",  "Advanced_DMZDLFilter_Content.asp", ""))                                                                                                                            
    	   //}       
    	   if (isModel() == 'WL500'|| isModel() == 'WL331' || isModel() == 'RT-N15')
    	   {
    	       appendChild(aux1, generateDocEntry(0, "<#t2MACFilter#>",  "Advanced_MACFilter_Content.asp", ""));
    	   }           
     }
     
     if (isModel() == 'WL550')    
     {      
      	  aux1 = appendChild(foldersTree, leafNode("t1Share"));
          appendChild(aux1, generateDocEntry(0, "t2ShareBasic", "Advanced_Wireless_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "t2ShareUser", "Advanced_UniqueKey_Content.asp", ""));
          
     	  aux1 = appendChild(foldersTree, leafNode("t1Disk"));
          appendChild(aux1, generateDocEntry(0, "t2DiskBasic", "Advanced_Wireless_Content.asp", ""));
          appendChild(aux1, generateDocEntry(0, "t2DiskStatus", "Advanced_UniqueKey_Content.asp", ""));
     }    
     
     if (isModel() != 'WL300' && isModel() != 'WL520gc' && isModel() != 'SnapAP' && isFlash() != '2MB' && isModel() != 'RT-N15')
     {
    	   aux1 = appendChild(foldersTree, leafNode("<#t1USB#>"));         
/*                      
           if (isModel() == 'WL550')
           {
//                appendChild(aux1, generateDocEntry(0, "<#t2FTP#>", "Advanced_USBStorage_Content.asp", ""))
           }
           else
           {
//           	appendChild(aux1, generateDocEntry(0, "<#t2FTP#>", "Advanced_USBStorage_Content.asp", ""))
           }
*/           
//           appendChild(aux1, generateDocEntry(0, "<#t2BC#>", "Advanced_StorageBasicConfig_Content.asp", ""))
//	   appendChild(aux1, generateDocEntry(0, "<#t2SN#>", "Advanced_StorageRight_Content.asp", ""))
	   appendChild(aux1, generateDocEntry(0, "<#t2BC#>", "Advanced_StorageRight_Content.asp", ""));
           appendChild(aux1, generateDocEntry(0, "<#t2UL#>", "Advanced_StorageUserList_Content.asp", ""));
//           appendChild(aux1, generateDocEntry(0, "<#t2Camera#>", "Advanced_WebCam_Content.asp", ""))           
     }
//Mark by Yen for test firmware
     if ((isModel() == 'WL500'||isModel() == 'RT-N15' )&& isFlash() != '2MB' && mode!='AP')
     {
     	  aux1 = appendChild(foldersTree, leafNode("<#t1QOS#>"));
	   appendChild(aux1, generateDocEntry(0, "<#t2QOS#>", "Advanced_QOS_Content.asp", ""));
           appendChild(aux1, generateDocEntry(0, "<#t2QOS2#>", "Advanced_QOSUserSpec_Content.asp", ""));
     }     
     else if (isModel3() == 'WL550gE')
     {
     	  aux1 = appendChild(foldersTree, leafNode("<#t1QOS#>"));
           appendChild(aux1, generateDocEntry(0, "<#t2QOS#>", "Advanced_QOS_Content.asp", ""));
     }
//end Yen
             
     aux1 = appendChild(foldersTree, leafNode("<#t1SYS#>"));
     if (isModel() != 'WL520gc' && isModel() != 'SnapAP')
     {
     	appendChild(aux1, generateDocEntry(0, "<#t2OP#>", "Advanced_OperationMode_Content.asp", "")) ;
     	appendChild(aux1, generateDocEntry(0, "<#t2Pass#>", "Advanced_Password_Content.asp",""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Upgrade#>", "Advanced_FirmwareUpgrade_Content.asp", ""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Setting#>", "Advanced_SettingBackup_Content.asp", ""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Factory#>", "Advanced_FactoryDefault_Content.asp", ""));
     }	
     else
     {
     	//appendChild(aux1, generateDocEntry(0, "<#t2OP#>", "Advanced_OperationMode_Content.asp", "")) 
     	appendChild(aux1, generateDocEntry(0, "<#t2Pass#>", "Advanced_Password_Content.asp",""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Upgrade#>", "Advanced_FirmwareUpgrade_Content.asp", ""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Setting#>", "Advanced_SettingBackup_Content.asp", ""));
     	appendChild(aux1, generateDocEntry(0, "<#t2Factory#>", "Advanced_FactoryDefault_Content.asp", ""));
     }
     //appendChild(aux1, generateDocEntry(0, "Printer Setup", "Advanced_PrinterSetup_Content.asp", ""))
    
     if (mode=='AP')
     { 
           aux1 = appendChild(foldersTree, leafNode("<#t1Status#>"));
           appendChild(aux1,generateDocEntry(0, "<#t2Status#>", "Main_AStatus_Content.asp", ""));
	if (isModel() != 'RT-N15') {
           appendChild(aux1,generateDocEntry(0, "<#t2UStatus#>", "Main_UStatus_Content.asp", ""));
	}
           appendChild(aux1,generateDocEntry(0, "<#t2WStatus#>", "Main_WStatus_Content.asp", ""));
           //appendChild(aux1,generateDocEntry(0, "<#t2Log#>", "Main_LogStatus_Content.asp", ""))
     }
     else
     {
        aux1 = appendChild(foldersTree, leafNode("<#t1Status#>"))
           appendChild(aux1,generateDocEntry(0, "<#t2Status#>", "Main_GStatus_Content.asp", ""));
	if (isModel() != 'RT-N15') {
           appendChild(aux1,generateDocEntry(0, "<#t2UStatus#>", "Main_UStatus_Content.asp", ""));
	}
           appendChild(aux1,generateDocEntry(0, "<#t2WStatus#>", "Main_WStatus_Content.asp", ""));     
           appendChild(aux1,generateDocEntry(0, "<#t2DStatus#>", "Main_DHCPStatus_Content.asp", ""));
           if (isModel() != 'SnapAP')
           {
              appendChild(aux1,generateDocEntry(0, "<#t2PortStatus#>", "Main_IPTStatus_Content.asp", ""));           
              appendChild(aux1, generateDocEntry(0, "<#t2RouteStatus#>", "Advanced_RouteStatus_Content.asp", ""));
              appendChild(aux1,generateDocEntry(0, "<#t2Log#>", "Main_LogStatus_Content.asp", ""));
           }   
     }                         
     aux1 = appendChild(foldersTree, leafNode("<#t1Logout#>"));
}
    
function clickOnFolderLink(folderName)
{
    mode = isMode();
    
    if (folderName == "<#t1Home#>")
    {
         if (mode == 'AP')
            //top.folderFrame.location="Basic_Operation_Content.asp#Wireless"
            top.folderFrame.location="Main_Index_AccessPoint.asp";
         else if (mode == 'Router')         
            //top.folderFrame.location="Basic_Operation_Content.asp#WANSetting"
            top.folderFrame.location="Main_Index_Router.asp";
         else
            //top.folderFrame.location="Basic_Operation_Content.asp#Wireless"         
            top.folderFrame.location="Main_Index_HomeGateway.asp";
    }
    else if (folderName == "<#t1Logout#>")
    {                  
         logout();
    }
    else if (folderName == "<#t1Quick#>")
    {
         if (mode == 'AP')
            //top.folderFrame.location="Basic_Operation_Content.asp#Wireless"
            top.folderFrame.location="Basic_AOperation_Content.asp";
         else if (mode == 'Router')         
            //top.folderFrame.location="Basic_Operation_Content.asp#WANSetting"
            top.folderFrame.location="Basic_ROperation_Content.asp";
         else
            //top.folderFrame.location="Basic_Operation_Content.asp#Wireless"         
            top.folderFrame.location="Basic_GOperation_Content.asp";
    }       
    else if (folderName == "<#t1Wireless#>")
    {
         top.folderFrame.location="Advanced_Wireless_Content.asp";
    }      
    else if (folderName == "<#t1IP#>" )
    {
         if (mode == 'AP')
           top.folderFrame.location="Advanced_APLAN_Content.asp";      
         else
           top.folderFrame.location="Advanced_LANWAN_Content.asp";
    }              
    else if (folderName == "<#t1IFW#>")
    {
         top.folderFrame.location="Advanced_BasicFirewall_Content.asp";
    }     
    else if (folderName == "<#t1WFW#>")
    {
         top.folderFrame.location="Advanced_DMZIP_Content.asp";
    }       
    else if (folderName == "<#t1USB#>")
    {
	top.folderFrame.location="Advanced_StorageRight_Content.asp";
           //top.folderFrame.location="Advanced_USBStorage_Content.asp"           
           //top.folderFrame.location="Advanced_WebCam_Content.asp"
    }     
    else if (folderName == "<#t1QOS#>")
    {
           top.folderFrame.location="Advanced_QOS_Content.asp";
           //top.folderFrame.location="Advanced_WebCam_Content.asp"
    }      
    else if (folderName == "<#t1Route#>")
    {
           top.folderFrame.location="Advanced_StaticRoute_Content.asp";
    }       
    else if (folderName == "<#t1NAT#>")
    {
    	  //if (isModel() == 'WL300')   
          //    top.folderFrame.location="Advanced_PortMapping_Content.asp"                      
          //else          
          top.folderFrame.location="Advanced_PortTrigger_Content.asp";
    }          
    else if (folderName == "<#t1SYS#>")
    {
//          if (isModel() == 'WL520' || isModel() == 'SnapAP')   
              top.folderFrame.location="Advanced_OperationMode_Content.asp";
//          else
//              top.folderFrame.location="Advanced_OperationMode_Content.asp"
    }          
    else if (folderName == "<#t1Status#>")
    {
         if (mode=='AP') 
                top.folderFrame.location="Main_AStatus_Content.asp";
         else if (mode=='Router')
                top.folderFrame.location="Main_RStatus_Content.asp";
         else        
                top.folderFrame.location="Main_GStatus_Content.asp";
    }            
}    

function clickOnFolderLinkClose(folderName)
{
    mode = isMode();
    
    if (folderName == "<#t1Quick#>")
    {
         if (mode == 'AP')
            top.folderFrame.location="Basic_Operation_Content.asp#Wireless";
         else if (mode == 'Router')
            top.folderFrame.location="Basic_Operation_Content.asp#WANSetting";
         else
            top.folderFrame.location="Basic_Operation_Content.asp#Broadband";
    }          
}    

// Auxiliary function to build the node
function folderNode(name)
{
    var arrayAux;
    
    arrayAux = new Array;
    arrayAux[0] = 1;
    arrayAux[1] = 1;
    arrayAux[2] = 0;
    arrayAux[3] = name;
            
    return arrayAux;
}

// Auxiliary function to build the node
// The entries in arrayAux[4]..array[length-1] are strings built in generate doc entry
function leafNode(name)
    {
    var arrayAux;
    arrayAux = new Array;
    arrayAux[0] = 0;
    arrayAux[1] = 0;
    arrayAux[2] = 1;
    arrayAux[3] = name;
        
    return arrayAux;
    }

//this way the generate tree function becomes simpler and less error prone
function appendChild(parent, child)
{    
    parent[parent.length] = child;
    return child;
}

//these are the last entries in the hierarchy, the local and remote links to html documents
function generateDocEntry(icon, docDescription, link)
    {
    var retString ="";
    var szDoc ="";
    if (icon==0)
        {
        retString = "<a href='"+link+"' class='wounderline' onClick='return top.checkChanged()' target=folderFrame><img src='./graph/doc.gif' alt='Opens in right frame'";
        szDoc = "<a href='" + link + "' class='wounderline' onClick='return top.checkChanged()' target=folderFrame>" + docDescription + "</a>";
        }
    else
        {
        retString = "<a href='http://"+link+"' class='wounderline' onClick='return top.checkChanged()' target=_blank><img src='./graph/link.gif' alt='Opens in new window'";
        szDoc = "<a href='" + link + "' class='wounderline' onClick='return top.checkChanged()' target=_blank>" + docDescription + "</a>";
        }
    retString = retString + " border=0></a><td nowrap><font size=-1 face='Arial, Helvetica'>" + szDoc + "</font>";

    return retString;
    }


// **********************
// display functions

//redraws the left frame
function redrawTree()
{
    var doc = top.treeFrame.window.document;

    mode = isMode();

    doc.clear();                 
    doc.write("<html>");
    doc.write("<head>");
    doc.write("<link rel='stylesheet' type='text/css' href='style.css' media='screen'>");
    doc.write("</head>");   
    doc.write("<body bgcolor='#660066' text='#FFFFFF'>");
    doc.write("<table  width='144' border='0' cellspacing='0' cellpadding='0' height='125'>");
    doc.write("<tr>");
    doc.write("<td>");
    doc.write("<div align='center'>");

    if (isModel()=="WL520gc" || isModel()=="SnapAP" || isModel()=="WL331")
    {
    	doc.write("<img src='graph/asusLogo.jpg' width='144' height='66'></div>");
    }
    else
    {
    	if (mode == 'AP')
       		doc.write("<img src='graph/asusLogo.jpg' width='144' height='66'></div>");
    	else if (mode == 'Gateway')
       		doc.write("<img src='graph/asusLogo.jpg' width='144' height='66'></div>");
    	else if (mode == 'Router')   
       		doc.write("<img src='graph/asusLogo.jpg' width='144' height='66'></div>");
/*
    	if (mode == 'AP')
       		doc.write("<img src='graph/asusLogoA.jpg' width='144' height='66'></div>");
    	else if (mode == 'Gateway')
       		doc.write("<img src='graph/asusLogoG.jpg' width='144' height='66'></div>");
    	else if (mode == 'Router')   
       		doc.write("<img src='graph/asusLogoR.jpg' width='144' height='66'></div>");
*/
    }   		
       
    doc.write("<div align='left'style='margin-left:3'>");
       
    doc.write("</td>");
    doc.write("</tr>");
    doc.write("</table>");      
    redrawNode(foldersTree, doc, 0, 1, "");
    doc.close();
    }

//recursive function over the tree structure called by redrawTree
function redrawNode(foldersNode, doc, level, lastNode, leftSide)
    {
    var j=0;
    var i=0;

if (level>0)
{
    doc.write("<table border=0 cellspacing=0 cellpadding=0>");
    doc.write("<tr><td valign = middle nowrap>");

    doc.write(leftSide);

    if (level>1)
        if (lastNode) //the last 'brother' in the children array
            {
            doc.write("<img src='./graph/lastnode.gif' width=16 height=22>");
            leftSide = leftSide + "<img src='./graph/blank.gif' width=16 height=22>";
		}
        else
            {
            doc.write("<img src='./graph/node.gif' width=16 height=22>");
            leftSide = leftSide + "<img src='./graph/vertline.gif' width=16 height=22>";
            }

    displayIconAndLabel(foldersNode, doc);
    doc.write("</table>");
}

    if (foldersNode.length > 4 && foldersNode[0]) //there are sub-nodes and the folder is open
        {
        if (!foldersNode[2]) //for folders with folders
            {
            level=level+1;
            for (i=4; i<foldersNode.length;i++)
                if (i==foldersNode.length-1)
                    redrawNode(foldersNode[i], doc, level, 1, leftSide);
                else
                    redrawNode(foldersNode[i], doc, level, 0, leftSide);
            }
        else //for folders with documents
            {
            for (i=4; i<foldersNode.length;i++)
                {
                doc.write("<table border=0 cellspacing=0 cellpadding=0 valign=center>");
                doc.write("<tr><td nowrap>");
                doc.write(leftSide);
                if (i==foldersNode.length - 1)
                    doc.write("<img src='./graph/lastnode.gif' width=16 height=22>");
                else
                    doc.write("<img src='./graph/node.gif' width=16 height=22>");
                doc.write(foldersNode[i]);
                doc.write("</table>");
                }
            }
        }
    }

//builds the html code to display a folder and its label
function displayIconAndLabel(foldersNode, doc)
    {
    var szOpenIconName = "./graph/openfolder.gif";
    var szCloseIconName = "./graph/closedfolder.gif";
    if (foldersNode[3] == "Access Point")
      {
      szOpenIconName = "./graph/apicon.gif";
      szCloseIconName = "./graph/apicon.gif";
      }
    doc.write("<a href='javascript:top.openBranch(\"" + foldersNode[3] + "\")' onClick='return top.checkChanged()'><img src=");
    if (foldersNode[1])
      doc.write(szOpenIconName + " width=22 height=22 border=noborder></a>");
    else
      doc.write(szCloseIconName + " width=22 height=22 border=noborder></a>");
    doc.write("<td valign=middle align=left nowrap>");
    doc.write("<a href='javascript:top.openBranch(\"" + foldersNode[3] + "\")'  onClick='return top.checkChanged()'>");
    doc.write("<font size=-1 face='Arial, Helvetica' class='wounderline'>"+foldersNode[3]+"</font></a>");
    }

//**********************+
// Recursive functions

//when a parent is closed all children also are
function closeFolders(foldersNode)
    {
    var i=0;

    if (!foldersNode[2])
        {
        for (i=4; i< foldersNode.length; i++)
            closeFolders(foldersNode[i]);
        }
    foldersNode[0] = 0;
    foldersNode[1] = 0;
    }

//recursive over the tree structure
//called by openbranch
function clickOnFolderRec(foldersNode, folderName)
{
    var i=0;
        
    if (foldersNode[3] == folderName)
    {
        if (foldersNode[0])
        {
            closeFolders(foldersNode);
        }    
        else
        {
            clickOnFolderLink(folderName);
            if (folderName != '<#t1Quick#>' && folderName != '<#t1Home#>' && folderName != '<#t1Logout#>' )
            {
               foldersNode[0] = 1;
               foldersNode[1] = 1;    
            }   
            else if (isModel() == 'WL550' && folderName == '<#t1Quick#>')
            {
               foldersNode[0] = 1;
               foldersNode[1] = 1;
            }
	    else if (folderName == '<#t1Logout#>')
	    {
            	if (logout_flag==1)
            	{
            		logout_flag=0;
			alert('<#JS_logout_alert#>');
			location="Logout.asp";
		}
	    }
            else
            {
               initializeTree();
            }                                                      
        }            
    }
    else
    {
        if (!foldersNode[2])
        {
            for (i=4; i< foldersNode.length; i++)
            {
                clickOnFolderRec(foldersNode[i], folderName);
            }    
        }       
    }
}

// ********************
// Event handlers

//called when the user clicks on a folder
function openBranch(branchName)
{
    clickOnFolderRec(foldersTree, branchName);
    if (branchName=="WL600 Setting" && foldersTree[0]==0)
    {
        top.folderFrame.location="Main_Index_Content.asp";
    }        
            
    timeOutId = setTimeout("redrawTree()",100);
 
}

//called after this html file is loaded
function initializeTree()
{   
    foldersTree = 0;
    generateTree();
    redrawTree();        
}

function checkChanged()
{
    if (pageChanged == '1')
    {
        if (pageChangedCount==6)
        {
    	   pageChangedCount = 0;
    	   ret = 1;
        } 
else if (pageChangedCount>2)
{
        ret_st ="<%nvram_get_x("","preferred_lang");%>";
        if(ret_st=='EN')
                ret = confirm('Please, please, please press "Apply" or "Finish" to confirm setting if you DO NOT ignore your changes!!!!!');
        else if(ret_st=='TW')
                ret = confirm('如果不想忽略您所做的改變, 請按下"套用"或"完成"以送出您做的設定');
        else if(ret_st == 'CN')
        	ret = confirm('若您不想忽略所作的变更，请按下“应用”或“完成”来确认设置！！');
 }
else
{
        ret_st="<%nvram_get_x("","preferred_lang");%>";
        if(ret_st=='EN')
                ret = confirm('Page has been modified, are you sure to ignore changes?');
        else if(ret_st =='TW')
                ret = confirm("頁面已被修改過, 確定要忽略改變？");
        else if(ret_st == 'CN')
        	ret = confirm('页面已被更改，您确定要忽略这些变更吗？')
}
        
        if (ret)
        {
           pageChanged = 0;
           pageChangedCount = 0;
        }   
	        else
        {
           pageChangedCount++;
        }

        return ret;
    }   
    else return true;
}

var foldersTree = 0;
var timeOutId = 0;
var pageChanged = 0;
var pageChangedCount = 0;
var goquick=0;
var logout_flag=0;
</script>

<frameset cols="120,506" border="0" framespacing="0" frameborder="0" onLoad="initializeTree()">
<noscript>
<p><b>You are using a browser which does not support JavaScript.</b>
<p>If you are using Netscape 2.01 or later then you can enable JavaScript.<br>
<p>Version 2 or 3: Choose Options|Network Preferences, choose the Languages tab, click Enable Javascript and then click OK.
<P>Version 4: Choose Edit|Preferences|Advanced, click Enable JavaScript and then click OK.
<br>
<P>Note: If you are using Internet Explorer and see this message, then your version does not support JavaScript.  (JavaScript cannot be disabled in Internet Explorer.) Upgrade to 3.1 or later.
<P><HR>
</noscript>
<frame src="Main_List.asp" marginwidth="10" marginheight="0" name="treeFrame" noresize>
<frameset rows="80,*" border="0" framespacing="0" on>
<frame src="Title.asp" marginwidth="0" marginheight="0" name="titleFrame" scrolling="no" noresize>
<frame src="<% urlcache(); %>" name="folderFrame" marginwidth="0" marginheight="0" noresize>
</frameset>
</frameset>
<noframes>
<body bgcolor="#FFFFFF" text="#000000"></body>
</noframes>
</html>
my file
