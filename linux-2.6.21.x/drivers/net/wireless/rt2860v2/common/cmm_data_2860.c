/*
   All functions in this file must be PCI-depended, or you should out your function
	in other files.

*/
#include	"rt_config.h"

extern RTMP_RF_REGS RF2850RegTable[];
extern UCHAR	NUM_OF_2850_CHNL;

USHORT RtmpPCI_WriteTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber)
{

	UCHAR			*pDMAHeaderBufVA;
	USHORT			TxIdx, RetTxIdx;
	PTXD_STRUC		pTxD;
	UINT32			BufBasePaLow;
	PRTMP_TX_RING	pTxRing;
	ULONG			SrcBufPA;
	USHORT			hwHeaderLen;


	SrcBufPA = PCI_MAP_SINGLE(pAd, (char *) pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, PCI_DMA_TODEVICE);

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	TxIdx = pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx;
	pDMAHeaderBufVA = (PUCHAR) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
	BufBasePaLow = RTMP_GetPhysicalAddressLow(pTxRing->Cell[TxIdx].DmaBuf.AllocPa);

	// copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer
	if (pTxBlk->TxFrameType == TX_AMSDU_FRAME)
	{
		hwHeaderLen = ROUND_UP(pTxBlk->MpduHeaderLen-LENGTH_AMSDU_SUBFRAMEHEAD, 4)+LENGTH_AMSDU_SUBFRAMEHEAD;
	}
	else
	{
		hwHeaderLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);
	}
	NdisMoveMemory(pDMAHeaderBufVA, pTxBlk->HeaderBuf, TXINFO_SIZE + TXWI_SIZE + hwHeaderLen);

	pTxRing->Cell[TxIdx].pNdisPacket = pTxBlk->pPacket;
	pTxRing->Cell[TxIdx].pNextNdisPacket = NULL;

	//
	// build Tx Descriptor
	// 

	pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
	NdisZeroMemory(pTxD, TXD_SIZE);

	pTxD->SDPtr0 = BufBasePaLow;
	pTxD->SDLen0 = TXINFO_SIZE + TXWI_SIZE + hwHeaderLen; // include padding
	pTxD->SDPtr1 = SrcBufPA;
	pTxD->SDLen1 = pTxBlk->SrcBufLen;
	pTxD->LastSec0 = 0;
	pTxD->LastSec1 = (bIsLast) ? 1 : 0;

	RTMPWriteTxDescriptor(pAd, pTxD, FALSE, FIFO_EDCA);

	RetTxIdx = TxIdx;
	//
	// Update Tx index
	// 
	INC_RING_INDEX(TxIdx, TX_RING_SIZE);
	pTxRing->TxCpuIdx = TxIdx;

	*FreeNumber -= 1;

	return RetTxIdx;
}


USHORT RtmpPCI_WriteSingleTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber)
{

	UCHAR			*pDMAHeaderBufVA;
	USHORT			TxIdx, RetTxIdx;
	PTXD_STRUC		pTxD;
#ifdef BIG_ENDIAN
    PTXD_STRUC      pDestTxD;
    TXD_STRUC       TxD;
#endif
	UINT32			BufBasePaLow;
	PRTMP_TX_RING	pTxRing;
	ULONG			SrcBufPA;
	USHORT			hwHeaderLen;


	SrcBufPA = PCI_MAP_SINGLE(pAd, (char *) pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, PCI_DMA_TODEVICE);

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	TxIdx = pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx;
	pDMAHeaderBufVA = (PUCHAR) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
	BufBasePaLow = RTMP_GetPhysicalAddressLow(pTxRing->Cell[TxIdx].DmaBuf.AllocPa);

	// copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer
	//hwHeaderLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);
	hwHeaderLen = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;

	NdisMoveMemory(pDMAHeaderBufVA, pTxBlk->HeaderBuf, TXINFO_SIZE + TXWI_SIZE + hwHeaderLen);

	pTxRing->Cell[TxIdx].pNdisPacket = pTxBlk->pPacket;
	pTxRing->Cell[TxIdx].pNextNdisPacket = NULL;

	//
	// build Tx Descriptor
	// 
#ifndef BIG_ENDIAN
	pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
#else
	pDestTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
	TxD = *pDestTxD;
	pTxD = &TxD;
#endif
	NdisZeroMemory(pTxD, TXD_SIZE);

	pTxD->SDPtr0 = BufBasePaLow;
	pTxD->SDLen0 = TXINFO_SIZE + TXWI_SIZE + hwHeaderLen; // include padding
	pTxD->SDPtr1 = SrcBufPA;
	pTxD->SDLen1 = pTxBlk->SrcBufLen;
	pTxD->LastSec0 = 0;
	pTxD->LastSec1 = (bIsLast) ? 1 : 0;

	RTMPWriteTxDescriptor(pAd, pTxD, FALSE, FIFO_EDCA);
#ifdef BIG_ENDIAN
	RTMPWIEndianChange((PUCHAR)(pDMAHeaderBufVA + TXINFO_SIZE), TYPE_TXWI);
	RTMPFrameEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA + TXINFO_SIZE + TXWI_SIZE), DIR_WRITE, FALSE);
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
    WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);	
#endif // BIG_ENDIAN //

	RetTxIdx = TxIdx;
	//
	// Update Tx index
	// 
	INC_RING_INDEX(TxIdx, TX_RING_SIZE);
	pTxRing->TxCpuIdx = TxIdx;

	*FreeNumber -= 1;

	return RetTxIdx;
}


USHORT RtmpPCI_WriteMultiTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			frameNum,
	OUT	USHORT			*FreeNumber)
{
	BOOLEAN bIsLast;
	UCHAR			*pDMAHeaderBufVA;
	USHORT			TxIdx, RetTxIdx;
	PTXD_STRUC		pTxD;
#ifdef BIG_ENDIAN
    PTXD_STRUC      pDestTxD;
    TXD_STRUC       TxD;
#endif
	UINT32			BufBasePaLow;
	PRTMP_TX_RING	pTxRing;
	ULONG			SrcBufPA;
	USHORT			hwHdrLen;
	UINT32			firstDMALen;

	bIsLast = ((frameNum == (pTxBlk->TotalFrameNum - 1)) ? 1 : 0);

	
	SrcBufPA = PCI_MAP_SINGLE(pAd, (char *) pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, PCI_DMA_TODEVICE);

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	TxIdx = pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx;
	pDMAHeaderBufVA = (PUCHAR) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
	BufBasePaLow = RTMP_GetPhysicalAddressLow(pTxRing->Cell[TxIdx].DmaBuf.AllocPa);

	if (frameNum == 0)
	{
		// copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer
		if (pTxBlk->TxFrameType == TX_AMSDU_FRAME)
			hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-LENGTH_AMSDU_SUBFRAMEHEAD, 4)+LENGTH_AMSDU_SUBFRAMEHEAD;
		else if (pTxBlk->TxFrameType == TX_RALINK_FRAME)
			hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-LENGTH_ARALINK_HEADER_FIELD, 4)+LENGTH_ARALINK_HEADER_FIELD;
		else
			hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);

		firstDMALen = TXINFO_SIZE + TXWI_SIZE + hwHdrLen;
	}
	else
	{
		firstDMALen = pTxBlk->MpduHeaderLen;
	}

	NdisMoveMemory(pDMAHeaderBufVA, pTxBlk->HeaderBuf, firstDMALen); 
		
	pTxRing->Cell[TxIdx].pNdisPacket = pTxBlk->pPacket;
	pTxRing->Cell[TxIdx].pNextNdisPacket = NULL;
	
	//
	// build Tx Descriptor
	// 
#ifndef BIG_ENDIAN
	pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
#else
	pDestTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
	TxD = *pDestTxD;
	pTxD = &TxD;
#endif
	NdisZeroMemory(pTxD, TXD_SIZE);

	pTxD->SDPtr0 = BufBasePaLow;
	pTxD->SDLen0 = firstDMALen; // include padding
	pTxD->SDPtr1 = SrcBufPA;
	pTxD->SDLen1 = pTxBlk->SrcBufLen;
	pTxD->LastSec0 = 0;
	pTxD->LastSec1 = (bIsLast) ? 1 : 0;

	RTMPWriteTxDescriptor(pAd, pTxD, FALSE, FIFO_EDCA);

#ifdef BIG_ENDIAN
	if (frameNum == 0)
		RTMPFrameEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA+ TXINFO_SIZE + TXWI_SIZE), DIR_WRITE, FALSE);
	
	if (frameNum != 0)
		RTMPWIEndianChange((PUCHAR)(pDMAHeaderBufVA + TXINFO_SIZE), TYPE_TXWI);
	
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
	WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);
#endif // BIG_ENDIAN //
	
	RetTxIdx = TxIdx;
	//
	// Update Tx index
	// 
	INC_RING_INDEX(TxIdx, TX_RING_SIZE);
	pTxRing->TxCpuIdx = TxIdx;

	*FreeNumber -= 1;

	return RetTxIdx;
	
}


VOID RtmpPCI_FinalWriteTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	USHORT			totalMPDUSize,
	IN	USHORT			FirstTxIdx)
{

	PTXWI_STRUC		pTxWI;
	PRTMP_TX_RING	pTxRing;

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	pTxWI = (PTXWI_STRUC) pTxRing->Cell[FirstTxIdx].DmaBuf.AllocVa;
	pTxWI->MPDUtotalByteCount = totalMPDUSize;
#ifdef BIG_ENDIAN
	RTMPWIEndianChange((PUCHAR)pTxWI, TYPE_TXWI);
#endif // BIG_ENDIAN //

}


VOID RtmpPCIDataLastTxIdx(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			QueIdx,
	IN	USHORT			LastTxIdx)
{
	PTXD_STRUC		pTxD;
#ifdef BIG_ENDIAN
    PTXD_STRUC      pDestTxD;
    TXD_STRUC       TxD;
#endif
	PRTMP_TX_RING	pTxRing;

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[QueIdx];

	//
	// build Tx Descriptor
	// 
#ifndef BIG_ENDIAN
	pTxD = (PTXD_STRUC) pTxRing->Cell[LastTxIdx].AllocVa;
#else
	pDestTxD = (PTXD_STRUC) pTxRing->Cell[LastTxIdx].AllocVa;
	TxD = *pDestTxD;
	pTxD = &TxD;
#endif

	pTxD->LastSec1 = 1;

#ifdef BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
    WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);
#endif // BIG_ENDIAN //

}


USHORT	RtmpPCI_WriteFragTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			fragNum,
	OUT	USHORT			*FreeNumber)
{
	UCHAR			*pDMAHeaderBufVA;
	USHORT			TxIdx, RetTxIdx;
	PTXD_STRUC		pTxD;
#ifdef BIG_ENDIAN
    PTXD_STRUC      pDestTxD;
    TXD_STRUC       TxD;
#endif
	UINT32			BufBasePaLow, SrcBufPA;
	PRTMP_TX_RING	pTxRing;
	USHORT			hwHeaderLen;
	UINT32			firstDMALen;


	//
	// Get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	TxIdx = pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx;
	pDMAHeaderBufVA = (PUCHAR) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
	BufBasePaLow = RTMP_GetPhysicalAddressLow(pTxRing->Cell[TxIdx].DmaBuf.AllocPa);

	//
	// Copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer
	//
	hwHeaderLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);

	firstDMALen = TXINFO_SIZE + TXWI_SIZE + hwHeaderLen;
	NdisMoveMemory(pDMAHeaderBufVA, pTxBlk->HeaderBuf, firstDMALen); 
		

	//
	// Build Tx Descriptor
	// 
#ifndef BIG_ENDIAN
	pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
#else
	pDestTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
	TxD = *pDestTxD;
	pTxD = &TxD;
#endif
	NdisZeroMemory(pTxD, TXD_SIZE);
	SrcBufPA = PCI_MAP_SINGLE(pAd, (char *)(pTxBlk->pSrcBufData), pTxBlk->SrcBufLen, PCI_DMA_TODEVICE);
	
	if (fragNum == pTxBlk->TotalFragNum)
	{
		pTxRing->Cell[TxIdx].pNdisPacket = pTxBlk->pPacket;
		pTxRing->Cell[TxIdx].pNextNdisPacket = NULL;
	}
	
	pTxD->SDPtr0 = BufBasePaLow;
	pTxD->SDLen0 = firstDMALen; // include padding
	pTxD->SDPtr1 = SrcBufPA;
	pTxD->SDLen1 = pTxBlk->SrcBufLen;
	pTxD->LastSec0 = 0;
	pTxD->LastSec1 = 1;

	RTMPWriteTxDescriptor(pAd, pTxD, FALSE, FIFO_EDCA);

#ifdef BIG_ENDIAN
	RTMPWIEndianChange((PUCHAR)(pDMAHeaderBufVA + TXINFO_SIZE), TYPE_TXWI);
	RTMPFrameEndianChange(pAd, (PUCHAR)(pDMAHeaderBufVA + TXINFO_SIZE + TXWI_SIZE), DIR_WRITE, FALSE);
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
    WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);	
#endif // BIG_ENDIAN //

	RetTxIdx = TxIdx;
	pTxBlk->Priv += pTxBlk->SrcBufLen;
	
	//
	// Update Tx index
	// 
	INC_RING_INDEX(TxIdx, TX_RING_SIZE);
	pTxRing->TxCpuIdx = TxIdx;

	*FreeNumber -= 1;

	return RetTxIdx;
	
}

#if 0
USHORT RtmpPCI_WriteSubTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber)
{

	UCHAR			*pDMAHeaderBufVA;
	USHORT			TxIdx;
	PTXD_STRUC		pTxD;
	UINT32			BufBasePaLow;
	PRTMP_TX_RING	pTxRing;
	ULONG			SrcBufPA;
	USHORT			LastTxIdx;


	SrcBufPA = PCI_MAP_SINGLE(pAd, (char *) pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, PCI_DMA_TODEVICE);

	//
	// get Tx Ring Resource
	// 
	pTxRing = &pAd->TxRing[pTxBlk->QueIdx];
	TxIdx = pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx;
	pDMAHeaderBufVA = (PUCHAR) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
	BufBasePaLow = RTMP_GetPhysicalAddressLow(pTxRing->Cell[TxIdx].DmaBuf.AllocPa);

	NdisMoveMemory(pDMAHeaderBufVA, pTxBlk->HeaderBuf, pTxBlk->MpduHeaderLen); 

	pTxRing->Cell[TxIdx].pNdisPacket = pTxBlk->pPacket;
	pTxRing->Cell[TxIdx].pNextNdisPacket = NULL;

	//
	// build Tx Descriptor
	// 

	pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
	NdisZeroMemory(pTxD, TXD_SIZE);

	pTxD->SDPtr0 = BufBasePaLow;
	pTxD->SDLen0 = pTxBlk->MpduHeaderLen; // include padding
	pTxD->SDPtr1 = SrcBufPA;
	pTxD->SDLen1 = pTxBlk->SrcBufLen;
	pTxD->LastSec0 = 0;
	pTxD->LastSec1 = (bIsLast) ? 1 : 0;

	RTMPWriteTxDescriptor(pAd, pTxD, FALSE, FIFO_EDCA);

	LastTxIdx = TxIdx;

	//
	// Update Tx index
	// 
	INC_RING_INDEX(TxIdx, TX_RING_SIZE);
	pTxRing->TxCpuIdx = TxIdx;

	*FreeNumber -= 1;

	return LastTxIdx;
}


VOID RtmpPCIDataKickOut(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			QueIdx)
{
	RTMP_IO_WRITE32(pAd, TX_CTX_IDX0+(pTxBlk->QueIdx*0x10), pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx);
}
#endif


/*
	Must be run in Interrupt context
	This function handle PCI specific TxDesc and cpu index update and kick the packet out.
 */
int RtmpPCIMgmtKickOut(
	IN RTMP_ADAPTER 	*pAd, 
	IN UCHAR 			QueIdx,
	IN PNDIS_PACKET		pPacket,
	IN PUCHAR			pSrcBufVA,
	IN UINT 			SrcBufLen)
{
	PTXD_STRUC		pTxD;
#ifdef BIG_ENDIAN
    PTXD_STRUC      pDestTxD;
    TXD_STRUC       TxD;
#endif
	ULONG			SwIdx = pAd->MgmtRing.TxCpuIdx;
	ULONG			SrcBufPA;

	
#ifdef BIG_ENDIAN
    pDestTxD  = (PTXD_STRUC)pAd->MgmtRing.Cell[SwIdx].AllocVa;
    TxD = *pDestTxD;
    pTxD = &TxD;
    RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
#else
	pTxD  = (PTXD_STRUC) pAd->MgmtRing.Cell[SwIdx].AllocVa;
#endif

	pAd->MgmtRing.Cell[SwIdx].pNdisPacket = pPacket;
	pAd->MgmtRing.Cell[SwIdx].pNextNdisPacket = NULL;

	SrcBufPA = PCI_MAP_SINGLE(pAd, pSrcBufVA, SrcBufLen, PCI_DMA_TODEVICE);
	
	RTMPWriteTxDescriptor(pAd, pTxD, TRUE, FIFO_MGMT);
	pTxD->LastSec0 = 1;
	pTxD->LastSec1 = 1;
	pTxD->DMADONE = 0;
	pTxD->SDLen0 = SrcBufLen;
	pTxD->SDLen1 = 0;
	pTxD->SDPtr0 = SrcBufPA;
#ifdef BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
    WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);
#endif
	
//==================================================================
/*	DBGPRINT_RAW(RT_DEBUG_TRACE, ("MLMEHardTransmit\n"));
	for (i = 0; i < (TXWI_SIZE+24); i++)
	{
	
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("%x:", *(pSrcBufVA+i)));
		if ( i%4 == 3)
			DBGPRINT_RAW(RT_DEBUG_TRACE, (" :: "));
		if ( i%16 == 15)
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("\n      "));
	}
	DBGPRINT_RAW(RT_DEBUG_TRACE, ("\n      "));*/
//=======================================================================

	pAd->RalinkCounters.KickTxCount++;
	pAd->RalinkCounters.OneSecTxDoneCount++;

	// Increase TX_CTX_IDX, but write to register later.
	INC_RING_INDEX(pAd->MgmtRing.TxCpuIdx, MGMT_RING_SIZE);

	RTMP_IO_WRITE32(pAd, TX_MGMTCTX_IDX,  pAd->MgmtRing.TxCpuIdx);

	return 0;
}


#ifdef CONFIG_STA_SUPPORT
/*
	========================================================================

	Routine Description:
		Check Rx descriptor, return NDIS_STATUS_FAILURE if any error dound
		
	Arguments:
		pRxD		Pointer to the Rx descriptor
		
	Return Value:
		NDIS_STATUS_SUCCESS 	No err
		NDIS_STATUS_FAILURE 	Error
		
	Note:
	
	========================================================================
*/
NDIS_STATUS RTMPCheckRxError(
	IN	PRTMP_ADAPTER		pAd, 
	IN	PHEADER_802_11		pHeader,
	IN	PRXWI_STRUC 		pRxWI,
	IN  PRT28XX_RXD_STRUC 	pRxD)
{
	PCIPHER_KEY pWpaKey;
	INT dBm;
	
	// Phy errors & CRC errors
	if (/*(pRxD->PhyErr) ||*/ (pRxD->Crc))
	{
		// Check RSSI for Noise Hist statistic collection.
		dBm = (INT) (pRxWI->RSSI0) - pAd->BbpRssiToDbmDelta;
		if (dBm <= -87)
			pAd->StaCfg.RPIDensity[0] += 1;
		else if (dBm <= -82)
			pAd->StaCfg.RPIDensity[1] += 1;
		else if (dBm <= -77)
			pAd->StaCfg.RPIDensity[2] += 1;
		else if (dBm <= -72)
			pAd->StaCfg.RPIDensity[3] += 1;
		else if (dBm <= -67)
			pAd->StaCfg.RPIDensity[4] += 1;
		else if (dBm <= -62)
			pAd->StaCfg.RPIDensity[5] += 1;
		else if (dBm <= -57)
			pAd->StaCfg.RPIDensity[6] += 1;
		else if (dBm > -57)
			pAd->StaCfg.RPIDensity[7] += 1;
		
		return(NDIS_STATUS_FAILURE);
	}
		
	// Add Rx size to channel load counter, we should ignore error counts
	pAd->StaCfg.CLBusyBytes += (pRxD->SDL0 + 14);

	// Drop ToDs promiscous frame, it is opened due to CCX 2 channel load statistics
	if (pHeader != NULL)
	{
#ifdef MESH_SUPPORT
		if (MESH_ON(pAd))
			;
		else
#endif // MESH_SUPPORT //
		if (pHeader->FC.ToDs)
		{
			return(NDIS_STATUS_FAILURE);
		}
	}

	// Drop not U2M frames, cant's drop here because we will drop beacon in this case
	// I am kind of doubting the U2M bit operation
	// if (pRxD->U2M == 0)
	//	return(NDIS_STATUS_FAILURE);

	// drop decyption fail frame
	if (pRxD->CipherErr)
	{
		if (pRxD->CipherErr == 2)
			{DBGPRINT_RAW(RT_DEBUG_TRACE,("pRxD ERROR: ICV ok but MICErr "));}
		else if (pRxD->CipherErr == 1)
			{DBGPRINT_RAW(RT_DEBUG_TRACE,("pRxD ERROR: ICV Err "));}
		else if (pRxD->CipherErr == 3)
			DBGPRINT_RAW(RT_DEBUG_TRACE,("pRxD ERROR: Key not valid "));
		
        if (((pRxD->CipherErr & 1) == 1) && pAd->CommonCfg.bWirelessEvent && INFRA_ON(pAd))
            RTMPSendWirelessEvent(pAd, IW_ICV_ERROR_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0); 
        
		DBGPRINT_RAW(RT_DEBUG_TRACE,(" %d (len=%d, Mcast=%d, MyBss=%d, Wcid=%d, KeyId=%d)\n", 
			pRxD->CipherErr,
			pRxD->SDL0, 
			pRxD->Mcast | pRxD->Bcast,
			pRxD->MyBss, 
			pRxWI->WirelessCliID, 
//			CipherName[pRxD->CipherAlg],
			pRxWI->KeyIndex));

		//
		// MIC Error
		//
		if (pRxD->CipherErr == 2)
		{			
			pWpaKey = &pAd->SharedKey[BSS0][pRxWI->KeyIndex];
#ifdef WPA_SUPPLICANT_SUPPORT                                    
            if (pAd->StaCfg.WpaSupplicantUP)
                WpaSendMicFailureToWpaSupplicant(pAd, 
                                   (pWpaKey->Type == PAIRWISEKEY) ? TRUE:FALSE);
            else
#endif // WPA_SUPPLICANT_SUPPORT //                             
			    RTMPReportMicError(pAd, pWpaKey);

            if (((pRxD->CipherErr & 2) == 2) && pAd->CommonCfg.bWirelessEvent && INFRA_ON(pAd))
                RTMPSendWirelessEvent(pAd, IW_MIC_ERROR_EVENT_FLAG, pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);

			DBGPRINT_RAW(RT_DEBUG_ERROR,("Rx MIC Value error\n"));
		}
		
		if (pHeader == NULL)
			return(NDIS_STATUS_SUCCESS);
		/*if ((pRxD->CipherAlg == CIPHER_AES) &&
			(pHeader->Sequence == pAd->FragFrame.Sequence))
		{
			//
			// Acceptable since the First FragFrame no CipherErr problem.
			//
			return(NDIS_STATUS_SUCCESS);
		}*/
		
		return(NDIS_STATUS_FAILURE);
	}
	
	return(NDIS_STATUS_SUCCESS);
}

/*
	==========================================================================
	Description:
		This routine sends command to firmware and turn our chip to power save mode.
		Both RadioOff and .11 power save function needs to call this routine.
	Input:
		Level = GUIRADIO_OFF  : GUI Radio Off mode
		Level = DOT11POWERSAVE  : 802.11 power save mode 
		Level = RTMP_HALT  : When Disable device. 
		
	==========================================================================
 */
VOID PCIeAsicRadioOff(
	IN PRTMP_ADAPTER    pAd,
	IN UCHAR            Level, 
	IN USHORT           TbttNumToNextWakeUp) 
{
	WPDMA_GLO_CFG_STRUC	DmaCfg;
	UCHAR	i;
	BOOLEAN		brc = FALSE, Cancelled;
    UINT32		TbTTTime = 0;
	UINT32		PsPollTime = 0, MACValue;
    ULONG		BeaconPeriodTime;
    UINT32		RxDmaIdx, RxCpuIdx;
	DBGPRINT(RT_DEBUG_TRACE, ("AsicRadioOff ===> TxCpuIdx = %d, TxDmaIdx = %d. RxCpuIdx = %d, RxDmaIdx = %d.\n", pAd->TxRing[0].TxCpuIdx, pAd->TxRing[0].TxDmaIdx, pAd->RxRing.RxCpuIdx, pAd->RxRing.RxDmaIdx));

    // Check Rx DMA busy status, if more than half is occupied, give up this radio off.
	RTMP_IO_READ32(pAd, RX_DRX_IDX , &RxDmaIdx);
	RTMP_IO_READ32(pAd, RX_CRX_IDX , &RxCpuIdx);
	if ((RxDmaIdx > RxCpuIdx) && ((RxDmaIdx - RxCpuIdx) > RX_RING_SIZE/4))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("AsicRadioOff ===> return1. RxDmaIdx = %d ,  RxCpuIdx = %d. \n", RxDmaIdx, RxCpuIdx));
		return;
	}
	else if ((RxCpuIdx >= RxDmaIdx) && ((RxCpuIdx - RxDmaIdx) < RX_RING_SIZE/4))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("AsicRadioOff ===> return2.  RxCpuIdx = %d. RxDmaIdx = %d ,  \n", RxCpuIdx, RxDmaIdx));
		return;
	}

    // Once go into this function, disable tx because don't want too many packets in queue to prevent HW stops.
	//pAd->bPCIclkOffDisableTx = TRUE;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
	{
	    RTMPCancelTimer(&pAd->Mlme.RadioOnOffTimer,	&Cancelled);
	    RTMPCancelTimer(&pAd->Mlme.PsPollTimer,	&Cancelled);

	    if (Level == DOT11POWERSAVE)
		{
			RTMP_IO_READ32(pAd, TBTT_TIMER, &TbTTTime);
			TbTTTime &= 0x1ffff;
			// 00. check if need to do sleep in this DTIM period.   If next beacon will arrive within 30ms , ...doesn't necessarily sleep.
			// TbTTTime uint = 64us, LEAD_TIME unit = 1024us, PsPollTime unit = 1ms
	        if  (((64*TbTTTime) <((LEAD_TIME*1024) + 40000)) && (TbttNumToNextWakeUp == 0))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("TbTTTime = 0x%x , give up this sleep. \n", TbTTTime));
	            OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
	            //pAd->bPCIclkOffDisableTx = FALSE;
				return;
			}
			else
			{
				PsPollTime = (64*TbTTTime- LEAD_TIME*1024)/1000;
				PsPollTime -= 3;

	            BeaconPeriodTime = pAd->CommonCfg.BeaconPeriod*102/100;
				if (TbttNumToNextWakeUp > 0)
					PsPollTime += ((TbttNumToNextWakeUp -1) * BeaconPeriodTime);
	            
	            pAd->Mlme.bPsPollTimerRunning = TRUE;
				RTMPSetTimer(&pAd->Mlme.PsPollTimer, PsPollTime);
			}
		}
	}
    
    // 0. Disable Tx DMA.
	RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
	DmaCfg.field.EnableTxDMA = 0;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, DmaCfg.word);

	// 1. Wait DMA not busy
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
		if (DmaCfg.field.TxDMABusy == 0)
			break;
		RTMPusecDelay(20);
		i++;
	}while(i < 50);

	if (i >= 50)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("DMA keeps busy.  return on PCIeAsicRadioOff ()\n"));
		//pAd->bPCIclkOffDisableTx = FALSE;
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
		DmaCfg.field.EnableTxDMA = 1;
		RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, DmaCfg.word);
		return;
	}

    RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);
    
    // Set to 1R.
	//RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, 0);
    
	// In Radio Off, we turn off RF clk, So now need to call ASICSwitchChannel again.
	if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel) 
		&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
	{	
		// Must using 40MHz.
		PCIeAsicTurnOffRFClk(pAd, pAd->CommonCfg.CentralChannel);
	}
	else
	{	
		// Must using 20MHz.
		PCIeAsicTurnOffRFClk(pAd, pAd->CommonCfg.Channel);
	}

    // When PCI clock is off, don't want to service interrupt.
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, AutoWakeupInt);
    
    RTMP_IO_WRITE32(pAd, RX_CRX_IDX, pAd->RxRing.RxCpuIdx);
	// Disable MAC Rx
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
	MACValue &= 0xf7;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);
    
	//  2. Send Sleep command 
#if 1 
	AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x00);   // send POWER-SAVE command to MCU. Timeout 40us.
#else // Code for advance power save with V0.10 firmware is not stable now	
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_STATUS, 0xffffffff);
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CID, 0xffffffff);    
	AsicSendCommandToMcu(pAd, 0x30, PowerSafeCID, 0xff, 0x00);   // send POWER-SAVE command to MCU. Timeout unit:40us.
	//  2-1. Wait command success
	//RTMPusecDelay(200);
	brc = AsicCheckCommanOk(pAd, PowerSafeCID);	

    if (brc == FALSE)
    {
        // try again
    	AsicSendCommandToMcu(pAd, 0x30, PowerSafeCID, 0xff, 0x00);   // send POWER-SAVE command to MCU. Timeout unit:40us.
    	//RTMPusecDelay(200);
    	brc = AsicCheckCommanOk(pAd, PowerSafeCID);	
    }
	
	//  3. After 0x30 command is ok, send radio off command. lowbyte = 0 for power safe.
	// If 0x30 command is not ok this time, we can ignore 0x35 command. It will make sure not cause firmware'r problem.
	if ((Level == DOT11POWERSAVE) && (brc == TRUE))
	{
		AsicSendCommandToMcu(pAd, 0x35, PowerRadioOffCID, 0, 0x00);	// lowbyte = 0 means to do power safe, NOT turn off radio.
	 	//  3-1. Wait command success
	 	AsicCheckCommanOk(pAd, PowerRadioOffCID);
	}
	else if (brc == TRUE)
	{
		AsicSendCommandToMcu(pAd, 0x35, PowerRadioOffCID, 1, 0x00);	// lowbyte = 0 means to do power safe, NOT turn off radio.
	 	//  3-1. Wait command success
	 	AsicCheckCommanOk(pAd, PowerRadioOffCID);
	}
#endif
    // Wait DMA not busy
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
		if (DmaCfg.field.RxDMABusy == 0)
			break;
		RTMPusecDelay(20);
		i++;
	}while(i < 50);

	if (i >= 50)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("DMA Rx keeps busy.  on PCIeAsicRadioOff ()\n"));
	}
	// disable DMA Rx.
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
		DmaCfg.field.EnableRxDMA = 0;
		RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, DmaCfg.word);
	}
    
	// Disable for stability. If PCIE Link Control is modified for advance power save, re-covery this code segment.
    //RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x1280);
    //OPSTATUS_SET_FLAG(pAd, fOP_STATUS_CLKSELECT_40MHZ);    
    
	if (Level == DOT11POWERSAVE)
	{
		AUTO_WAKEUP_STRUC	AutoWakeupCfg;
		//RTMPSetTimer(&pAd->Mlme.PsPollTimer, 90);
			
		// we have decided to SLEEP, so at least do it for a BEACON period.
		if (TbttNumToNextWakeUp == 0)
			TbttNumToNextWakeUp = 1;

		//AutoWakeupCfg.word = 0;
		//RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);

		// 1. Set auto wake up timer.
		AutoWakeupCfg.field.NumofSleepingTbtt = TbttNumToNextWakeUp - 1;
		AutoWakeupCfg.field.EnableAutoWakeup = 1;
		AutoWakeupCfg.field.AutoLeadTime = LEAD_TIME;
		RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);
	}
	
	//  4-1. If it's to disable our device. Need to restore PCI Configuration Space to its original value.
	if (brc == TRUE)
    {   
        if (i < 50)
            RTMPPCIeLinkCtrlSetting(pAd, 3);
#if 0        
    	if (Level == RTMP_HALT)
    	{
    		RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_HALT);
    	}
    	//  4. Set PCI configuration Space Link Comtrol fields.  Only Radio Off needs to call this function
    	else
    	{
    		RTMPPCIeLinkCtrlSetting(pAd, 3);
    	}
#endif        
    }

    //pAd->bPCIclkOffDisableTx = FALSE;
}


/*
	==========================================================================
	Description:
		This routine sends command to firmware and turn our chip to wake up mode from power save mode.
		Both RadioOn and .11 power save function needs to call this routine.
	Input:
		Level = GUIRADIO_OFF : call this function is from Radio Off to Radio On.  Need to restore PCI host value.
		Level = other value : normal wake up function.

	==========================================================================
 */
BOOLEAN PCIeAsicRadioOn(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR     Level)
{
    WPDMA_GLO_CFG_STRUC	DmaCfg;
	BOOLEAN				Cancelled, brv = TRUE;   
    UINT32			    MACValue;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
	{
	    pAd->Mlme.bPsPollTimerRunning = FALSE;
		RTMPCancelTimer(&pAd->Mlme.PsPollTimer,	&Cancelled);
	    // Need to check bPCIclkOffCommand flag. Because sometimes miss RTMPPCIePowerLinkCtrlRestore function before call this RadioOn from
		// autowakeup interrupt in EeePC.
		if ((Level == GUIRADIO_OFF) || (Level == GUI_IDLE_POWER_SAVE) || (pAd->bPCIclkOffCommand == TRUE))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("PCIeAsicRadioOn ()\n"));
			// 1. Set PCI Link Control in Configuration Space.
			RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_WAKEUP);
			RTMPusecDelay(6000);
		}
	}
    
    pAd->bPCIclkOff = FALSE;
	// Disable for stability. If PCIE Link Control is modified for advance power save, re-covery this code segment.
   	//RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x1a80);
    //OPSTATUS_SET_FLAG(pAd, fOP_STATUS_CLKSELECT_40MHZ);
    
	// 2. Send wake up command.
#if 1
	AsicSendCommandToMcu(pAd, 0x31, 0xff, 0x00, 0x00);
#else // Code for advance power save with V0.10 firmware is not stable now
	AsicSendCommandToMcu(pAd, 0x31, PowerWakeCID, 0x00, 0x00);
    
	// 2-1. wait command ok.
	brv = AsicCheckCommanOk(pAd, PowerWakeCID);	
#endif
    if (brv)
    {
    	//RTMP_IO_WRITE32(pAd, INT_MASK_CSR, (DELAYINTMASK|RxINT));
    	NICEnableInterrupt(pAd);
        
    	// 3. Enable Tx DMA.
    	RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
    	DmaCfg.field.EnableTxDMA = 1;
        DmaCfg.field.EnableRxDMA = 1;
    	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, DmaCfg.word);

        // Eable MAC Rx
    	RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
    	MACValue |= 0x8;
    	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);

    	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);
    	if (Level == GUI_IDLE_POWER_SAVE)
    	{
    		// In Radio Off, we turn off RF clk, So now need to call ASICSwitchChannel again.
    		if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel) 
    			&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
    		{	
    			// Must using 40MHz.
    			AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
    			AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
    		}
    		else
    		{	
    			// Must using 20MHz.
    			AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
    			AsicLockChannel(pAd, pAd->CommonCfg.Channel);
    		}
    	}
        return TRUE;
    }
    else 
        return FALSE;
}

VOID PCIeAsicTurnOffRFClk(
	IN PRTMP_ADAPTER pAd, 
	IN	UCHAR			Channel) 
{

	// RF R2 bit 18 = 0
	UINT32			R1 = 0, R2 = 0, R3 = 0;
	UCHAR			index;
	RTMP_RF_REGS	*RFRegTable;

	RFRegTable = RF2850RegTable;

	switch (pAd->RfIcType)
	{
		case RFIC_2820:
		case RFIC_2850:
		case RFIC_2720:
		case RFIC_2750:
			
			for (index = 0; index < NUM_OF_2850_CHNL; index++)
			{
				if (Channel == RFRegTable[index].Channel)
				{
					R1 = RFRegTable[index].R1 & 0xffffdfff;	// 
					R2 = RFRegTable[index].R2 & 0xfffbffff;	// 
					R3 = RFRegTable[index].R3 & 0xfff3ffff;	// 

					RTMP_RF_IO_WRITE32(pAd, R1);
					RTMP_RF_IO_WRITE32(pAd, R2);

					// Program R1b13 to 1, R3/b18,19 to 0, R2b18 to 0. 
					// Set RF R2 bit18=0, R3 bit[18:19]=0
					//if (pAd->StaCfg.bRadio == FALSE)
					if (1)
					{
						RTMP_RF_IO_WRITE32(pAd, R3);

						DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x,  R3 = 0x%08x \n",
							Channel, pAd->RfIcType, R2, R3));
					}
					else
						DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x \n",
							Channel, pAd->RfIcType, R2));
					break;
				}
			}
			break;

		default:
			break;
	}
}


VOID PCIeAsicTurnOnRFClk(
	IN PRTMP_ADAPTER pAd, 
	IN	UCHAR			Channel) 
{

	// RF R2 bit 18 = 0
	UINT32			R1 = 0, R2 = 0, R3 = 0;
	UCHAR			index;
	RTMP_RF_REGS	*RFRegTable;

	RFRegTable = RF2850RegTable;

	switch (pAd->RfIcType)
	{
		case RFIC_2820:
		case RFIC_2850:
		case RFIC_2720:
		case RFIC_2750:
			
			for (index = 0; index < NUM_OF_2850_CHNL; index++)
			{
				if (Channel == RFRegTable[index].Channel)
				{
					R3 = pAd->LatchRfRegs.R3;
					R3 &= 0xfff3ffff;
					R3 |= 0x00080000;
					RTMP_RF_IO_WRITE32(pAd, R3);
					
					R1 = RFRegTable[index].R1;
					RTMP_RF_IO_WRITE32(pAd, R1);

					R2 = RFRegTable[index].R2;
					if (pAd->Antenna.field.TxPath == 1)
					{
						R2 |= 0x4000;	// If TXpath is 1, bit 14 = 1;
					}
					
					if (pAd->Antenna.field.RxPath == 2)
					{
						R2 |= 0x40;	// write 1 to off Rxpath.
					}
					else if (pAd->Antenna.field.RxPath == 1)
					{
						R2 |= 0x20040;	// write 1 to off RxPath
					}
					RTMP_RF_IO_WRITE32(pAd, R2);
					
					break;
				}
			}
			break;

		default:
			break;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOnRFClk#%d(RF=%d, ) , R2=0x%08x\n",
		Channel, 
		pAd->RfIcType, 
		R2));
}

VOID RT28xxPciStaAsicForceWakeup(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN       bFromTx)
{
    AUTO_WAKEUP_STRUC	AutoWakeupCfg;
//    UINT32              Value = 0;

    if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
        return;

    if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WAKEUP_NOW))
    {
        DBGPRINT(RT_DEBUG_TRACE, ("waking up now!\n"));
        return;
    }

    //AutoWakeupCfg.word = 0;
	//RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);
    OPSTATUS_SET_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);
    
    //if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
    {
        // Support PCIe Advance Power Save
    	if (bFromTx == TRUE)
    	{
            pAd->Mlme.bPsPollTimerRunning = FALSE;
    		RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_WAKEUP);
    		RTMPusecDelay(3000);
            DBGPRINT(RT_DEBUG_TRACE, ("=======AsicForceWakeup===bFromTx\n"));
    	}

#if 0 //snowpin: ping packet lost test
	    PCIeAsicRadioOn(pAd, DOT11POWERSAVE);
#else
        if (PCIeAsicRadioOn(pAd, DOT11POWERSAVE))
        {
            // In Radio Off, we turn off RF clk, So now need to call ASICSwitchChannel again.
        	if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel) 
        		&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
        	{	
        		// Must using 40MHz.
        		AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
        		AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
        	}
        	else
        	{	
        		// Must using 20MHz.
        		AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
        		AsicLockChannel(pAd, pAd->CommonCfg.Channel);
        	}
        }
#endif		
    }
    /*else
    {
        // PCI, 2860-PCIe
        AsicSendCommandToMcu(pAd, 0x31, 0xff, 0x00, 0x00);
        //NICEnableInterrupt(pAd);
    }*/
    
    OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
    AutoWakeupCfg.word = 0;
	RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);
    OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);
    DBGPRINT(RT_DEBUG_TRACE, ("<=======RT28xxPciStaAsicForceWakeup\n"));
}

VOID RT28xxPciStaAsicSleepThenAutoWakeup(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TbttNumToNextWakeUp) 
{
    /*if (pAd->StaCfg.bRadio == FALSE)
	{
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		return;
	}*/
    //if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
    {
        if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WAKEUP_NOW))
        {
            DBGPRINT(RT_DEBUG_TRACE, ("waking up now!\n"));
            OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
            return;
        }
        PCIeAsicRadioOff(pAd, DOT11POWERSAVE, TbttNumToNextWakeUp);
    }
    /*else
    {
        AUTO_WAKEUP_STRUC	AutoWakeupCfg;	
        // we have decided to SLEEP, so at least do it for a BEACON period.	
        if (TbttNumToNextWakeUp == 0)
            TbttNumToNextWakeUp = 1; 
        
	    //RTMP_IO_WRITE32(pAd, INT_MASK_CSR, AutoWakeupInt);
        
        AutoWakeupCfg.word = 0; 
        RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);  
        AutoWakeupCfg.field.NumofSleepingTbtt = TbttNumToNextWakeUp - 1;    
        AutoWakeupCfg.field.EnableAutoWakeup = 1;   
        AutoWakeupCfg.field.AutoLeadTime = 5;   
        RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);  
        AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x00);   // send POWER-SAVE command to MCU. Timeout 40us.
        DBGPRINT(RT_DEBUG_TRACE, ("<-- %s, TbttNumToNextWakeUp=%d \n", __FUNCTION__, TbttNumToNextWakeUp));
    }*/
    OPSTATUS_SET_FLAG(pAd, fOP_STATUS_DOZE);
}

VOID PsPollWakeExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

    DBGPRINT(RT_DEBUG_TRACE,("-->PsPollWakeExec \n"));
    if (pAd->Mlme.bPsPollTimerRunning)
    {
	    RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_WAKEUP);
    }
    pAd->Mlme.bPsPollTimerRunning = FALSE;
}

VOID  RadioOnExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;
	WPDMA_GLO_CFG_STRUC	DmaCfg;
	BOOLEAN				Cancelled;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		DBGPRINT(RT_DEBUG_TRACE,("-->RadioOnExec() return on fOP_STATUS_DOZE == TRUE; \n"));
		RTMPSetTimer(&pAd->Mlme.RadioOnOffTimer, 10);
		return;
	}
	
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
	{
		DBGPRINT(RT_DEBUG_TRACE,("-->RadioOnExec() return on SCAN_IN_PROGRESS; \n"));
		RTMPSetTimer(&pAd->Mlme.RadioOnOffTimer, 10);
		return;
	}
    pAd->Mlme.bPsPollTimerRunning = FALSE;
	RTMPCancelTimer(&pAd->Mlme.PsPollTimer,	&Cancelled);
	if (pAd->StaCfg.bRadio == TRUE)
	{
		pAd->bPCIclkOff = FALSE;
        RTMPRingCleanUp(pAd, QID_AC_BK);
		RTMPRingCleanUp(pAd, QID_AC_BE);
		RTMPRingCleanUp(pAd, QID_AC_VI);
		RTMPRingCleanUp(pAd, QID_AC_VO);
		RTMPRingCleanUp(pAd, QID_HCCA);
		RTMPRingCleanUp(pAd, QID_MGMT);
		RTMPRingCleanUp(pAd, QID_RX);

        
		// Disable for stability. If PCIE Link Control is modified for advance power save, re-covery this code segment.
  		// Turn Off PDB clk
   		//RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0x1a80);
        //OPSTATUS_SET_FLAG(pAd, fOP_STATUS_CLKSELECT_40MHZ);
		
		// 2. Send wake up command.
		AsicSendCommandToMcu(pAd, 0x31, PowerWakeCID, 0x00, 0x02);   
		// 2-1. wait command ok.
		AsicCheckCommanOk(pAd, PowerWakeCID);	
        
		// When PCI clock is off, don't want to service interrupt. So when back to clock on, enable interrupt.
		//RTMP_IO_WRITE32(pAd, INT_MASK_CSR, (DELAYINTMASK|RxINT));
		NICEnableInterrupt(pAd);
		
		// 3. Enable Tx DMA.
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &DmaCfg.word);
		DmaCfg.field.EnableTxDMA = 1;
		RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, DmaCfg.word);
		
		// In Radio Off, we turn off RF clk, So now need to call ASICSwitchChannel again.
		if (INFRA_ON(pAd) && (pAd->CommonCfg.CentralChannel != pAd->CommonCfg.Channel) 
			&& (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40))
		{	
			// Must using 40MHz.
			AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
		}
		else
		{	
			// Must using 20MHz.
			AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
			AsicLockChannel(pAd, pAd->CommonCfg.Channel);
		}

		// Clear Radio off flag
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

		// Set LED
		RTMPSetLED(pAd, LED_RADIO_ON);

        if (pAd->StaCfg.Psm == PWR_ACTIVE)
        {
    		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, pAd->StaActive.BBPR3);
			
            // Turn clk to 80Mhz.
            // Disable for stability. If PCIE Link Control is modified for advance power save, re-covery this code segment.
            //RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe80);
            //OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_CLKSELECT_40MHZ);
        }
	}
	else
	{
		PCIeAsicRadioOff(pAd, GUIRADIO_OFF, 0);
	}
}

#endif // CONFIG_STA_SUPPORT //

VOID RT28xxPciMlmeRadioOn(
	IN PRTMP_ADAPTER pAd)
{    
    if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
   		return;
    
    DBGPRINT(RT_DEBUG_TRACE,("%s===>\n", __FUNCTION__));   
    
    if ((pAd->OpMode == OPMODE_AP) ||
        ((pAd->OpMode == OPMODE_STA) && (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))))
    {
	    NICResetFromError(pAd);

    	RTMPRingCleanUp(pAd, QID_AC_BK);
    	RTMPRingCleanUp(pAd, QID_AC_BE);
    	RTMPRingCleanUp(pAd, QID_AC_VI);
    	RTMPRingCleanUp(pAd, QID_AC_VO);
    	RTMPRingCleanUp(pAd, QID_HCCA);
    	RTMPRingCleanUp(pAd, QID_MGMT);
    	RTMPRingCleanUp(pAd, QID_RX);

    	// Enable Tx/Rx
    	RTMPEnableRxTx(pAd);
    	
    	// Clear Radio off flag
    	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

	    // Set LED
	    RTMPSetLED(pAd, LED_RADIO_ON);
    }

#ifdef CONFIG_STA_SUPPORT
    if ((pAd->OpMode == OPMODE_STA) &&
        (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE)))
    {
        BOOLEAN		Cancelled;
    	
    	RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_WAKEUP);

        pAd->Mlme.bPsPollTimerRunning = FALSE;
    	RTMPCancelTimer(&pAd->Mlme.PsPollTimer,	&Cancelled);
    	RTMPCancelTimer(&pAd->Mlme.RadioOnOffTimer,	&Cancelled);
    	RTMPSetTimer(&pAd->Mlme.RadioOnOffTimer, 10);
    }
#endif // CONFIG_STA_SUPPORT //
}

VOID RT28xxPciMlmeRadioOFF(
	IN PRTMP_ADAPTER pAd)
{
    WPDMA_GLO_CFG_STRUC	GloCfg;
	UINT32	Value = 0, i;
    
    if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
    	return;
    
    DBGPRINT(RT_DEBUG_TRACE,("%s===>\n", __FUNCTION__));
            
	// Set LED
	RTMPSetLED(pAd, LED_RADIO_OFF);
	// Set Radio off flag
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
    {
        if (pAd->pci_dev != NULL)
        {
            BOOLEAN Cancelled;
            pAd->Mlme.bPsPollTimerRunning = FALSE;
            RTMPCancelTimer(&pAd->Mlme.PsPollTimer,	&Cancelled);
	        RTMPCancelTimer(&pAd->Mlme.RadioOnOffTimer,	&Cancelled);
        }

        // Link down first if any association exists	
        if (INFRA_ON(pAd) || ADHOC_ON(pAd))		
            LinkDown(pAd, FALSE);   
        RTMPusecDelay(10000);   
        //==========================================    
        // Clean up old bss table   
        BssTableInit(&pAd->ScanTab);

        if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
        {
            RTMPSetTimer(&pAd->Mlme.RadioOnOffTimer, 10);
            return;
        }
    }
#endif // CONFIG_STA_SUPPORT //

	// Disable Tx/Rx DMA
	RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);	   // disable DMA 
	GloCfg.field.EnableTxDMA = 0;
	GloCfg.field.EnableRxDMA = 0;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);	   // abort all TX rings

	// Disable MAC Tx/Rx
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
	Value &= (0xfffffff3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);

	// Waiting for DMA idle
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0) && (GloCfg.field.RxDMABusy == 0))
			break;
		
		RTMPusecDelay(1000);
	}while (i++ < 100);


}
