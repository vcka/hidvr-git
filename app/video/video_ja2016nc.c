
#define VI_CIF_WIDTH (352)
#define VI_CIF_HEIGHT (VIDEO_IS_PAL() ? 288 : 240)

#define VI_QCIF_WIDTH (VI_CIF_WIDTH / 2)
#define VI_QCIF_HEIGHT (VI_CIF_HEIGHT / 2)

#define SPECIAL_D1_CH (MAX_CAM_CH + 1) // 24 for logo output 25 for d1 output

static unsigned int s_nLayerWidth = 0;
static unsigned int s_nLayerHeight = 0;

static inline void VI_D1_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 704;
	pAttr->stCapRect.s32X = VI_X_OFFSET;
	pAttr->enCapSel = VI_CAPSEL_BOTH;
	pAttr->bDownScale = HI_FALSE;
}

static inline void VI_CIF_ATTR_CONF(VI_CHN_ATTR_S* pAttr)
{
	pAttr->stCapRect.u32Width = 704;
	pAttr->stCapRect.s32X = VI_X_OFFSET;
	pAttr->enCapSel = VI_CAPSEL_BOTTOM;
	pAttr->bDownScale = HI_TRUE;
}

static void video_DisplayD1(int nVoDev, int nChn, unsigned char bDisplay)
{
	static int s_nDispViDev = -1;
	static int s_nDispViChn = -1;
	static int s_nDispVoChn = -1;
	VI_CHN_ATTR_S stViChnAttr;
	
	// dont try to change input the original setting of d1 channel
	if(bDisplay){
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			if(s_nDispVoChn >= MAX_D1_CNT){
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_CIF_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
			}
			DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTTOM));
		}
		if(nChn >= MAX_D1_CNT){
			if(0 == VIMAP_Get(nChn, &s_nDispViDev, &s_nDispViChn)){
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_D1_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				s_nDispVoChn = nChn;
			}
		}
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTH));
	}else{
		if(!(-1 == s_nDispViDev && -1 == s_nDispViChn)){
			if(nChn >= MAX_D1_CNT){
				// reset field setting
				DVR_ASSERT(HI_MPI_VI_GetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));
				VI_CIF_ATTR_CONF(&stViChnAttr);
				DVR_ASSERT(HI_MPI_VI_SetChnAttr(s_nDispViDev, s_nDispViChn, &stViChnAttr));	
				s_nDispViDev = -1;
				s_nDispViChn = -1;	
			}
			s_nDispVoChn = -1;
		}
		DVR_ASSERT(HI_MPI_VO_SetChnField(nVoDev, nChn, VO_FIELD_BOTTOM));
	}
}

void video_ChnSpeedRate(int Chn, int nRate)
{
}

void video_VoSpeedRate(int nRate)
{
}

void video_ChnStopOutput(int Chn)
{
}

void video_ChnResumeOutput(int Chn)
{
}

void video_VoOutputOpa(int nEnable)
{
}

static void video_InputInit()
{
    int ii, jj;

    VI_PUB_ATTR_S stViDevAttr;
    VI_CHN_ATTR_S stViChnAttr;

    // vi0 1-4   ch cif input 4d1
    // vi1 5-8   ch cif input 4d1
    // vi2 9-12  ch cif input 4d1
    // vi3 13-16 ch cif input 4d1
    ////////////////////////////////////////////////////////////////
    // vi
    stViDevAttr.enInputMode = VI_MODE_BT656;
    stViDevAttr.enWorkMode = VI_WORK_MODE_4D1;
    stViDevAttr.enViNorm = (VIDEO_IS_PAL() ? VIDEO_ENCODING_MODE_PAL : VIDEO_ENCODING_MODE_NTSC);
    // 4d1 input
    stViChnAttr.stCapRect.u32Width  = 704;
    stViChnAttr.stCapRect.u32Height = VI_CIF_HEIGHT;
    stViChnAttr.stCapRect.s32X = VI_X_OFFSET;
    stViChnAttr.stCapRect.s32Y = 0;
    stViChnAttr.bChromaResample = HI_FALSE;
    stViChnAttr.enCapSel = VO_FIELD_BOTTOM;
    stViChnAttr.bDownScale = HI_TRUE;
    stViChnAttr.bHighPri = HI_FALSE;
    stViChnAttr.enViPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    for(ii = 0; ii < 4; ii ++) {
        // interface
        DVR_ASSERT(HI_MPI_VI_Disable(ii));
        DVR_ASSERT(HI_MPI_VI_SetPubAttr(ii, &stViDevAttr));
        DVR_ASSERT(HI_MPI_VI_Enable(ii));
        for(jj = 0; jj < 4; jj ++) {
            DVR_ASSERT(HI_MPI_VI_SetChnAttr(ii, jj, &stViChnAttr));
            DVR_ASSERT(HI_MPI_VI_EnableChn(ii, jj));
            DVR_ASSERT(HI_MPI_VI_SetSrcFrameRate(ii, jj, MAX_CIF_LIVE_FPS));
            DVR_ASSERT(HI_MPI_VI_SetFrameRate(ii, jj, MAX_CIF_LIVE_FPS));
        }
    }
}

static void video_InputDestroy()
{
    int ii, jj;

    // vi0 1-4   ch cif input 4d1
    // vi1 5-8   ch cif input 4d1
    // vi2 9-12  ch cif input 4d1
    // vi3 13-16 ch cif input 4d1
    ////////////////////////////////////////////////////////////////
    for(ii = 0; ii < 4; ii ++) {
        for(jj = 0; jj < 4; ++ jj){
            DVR_ASSERT(HI_MPI_VI_DisableChn(ii, jj));
        }
        DVR_ASSERT(HI_MPI_VI_Disable(ii));
    }
}

static void video_Division(int nVoDev, int nScrWidth, int nScrHeight, int nDiv, int nPage, unsigned char bD1)
{
	int i = 0;
	int nGrid = 1;
	VO_CHN_ATTR_S stVoChnAttr;

	int nLocate = 0;
	int nChWidth = 0;
	int nChHeight = 0;
	int nOffsetX = (VO_ORIGIN_WIDTH - nScrWidth) / 2;
	int nOffsetY = (VO_ORIGIN_HEIGHT - nScrHeight) / 2;
	int nBlankW = 0;
	int nBlankH = 0;

	if(-1 == nDiv){
		nDiv = VIDEO_GetMaxDiv();
	}
	switch(nDiv)
	{
	case 1: nGrid = 1; break;
	case 4: nGrid = 2; break;
	case 9: nGrid = 3; break;
	case 16: nGrid = 4; break;
	case 25: nGrid = 5; break;
	default:
		return;
	}
	
	if(nVoDev == VO_CVBS_DEVICE) {
		nOffsetX = s_stVideo.ScrnEdges[1].nEdgeX;
		nOffsetY = s_stVideo.ScrnEdges[1].nEdgeY;

		nScrWidth  = s_stVideo.ScrnEdges[1].nEdgeW;
		nScrHeight = s_stVideo.ScrnEdges[1].nEdgeH;
	}
	
	nChWidth = nScrWidth / nGrid;
	nChHeight = nScrHeight / nGrid;
	nChWidth &= ~(2-1); // 2 alignment
	nChHeight &= ~(2-1); // 2 alignment
	nBlankW = nScrWidth % nChWidth;
	nBlankH = nScrHeight % nChHeight;
	for(i = 0, nLocate = 0; i < VIDEO_GetMaxDiv(); ++i){
		stVoChnAttr.stRect.s32X = nOffsetX + (((nLocate % nDiv) % nGrid) * nChWidth) & ~(2-1); // 2 alignment
		stVoChnAttr.stRect.s32Y = nOffsetY + (((nLocate % nDiv) / nGrid) * nChHeight) & ~(2-1); // 2 alignment	
		//VIDEO_TRACE("vo(%d,%d) (x,y)=%d,%d, [w,h]=%d,%d", nVoDev, i, stVoChnAttr.stRect.s32X, stVoChnAttr.stRect.s32Y, stVoChnAttr.stRect.u32Width, stVoChnAttr.stRect.u32Height);
		if((nGrid - 1) == (nLocate % nGrid)){
			// the right side
			stVoChnAttr.stRect.u32Width = nChWidth + nBlankW;
		}else{
			// for osd bar
			stVoChnAttr.stRect.u32Width = nChWidth - 2;
		}
		if((nGrid - 1) == (nLocate / nGrid)){
			stVoChnAttr.stRect.u32Height = nChHeight + nBlankH;
		}else{
			// for osd bar
			stVoChnAttr.stRect.u32Height = nChHeight - 2;
		}
		stVoChnAttr.u32Priority = 1;
		stVoChnAttr.bZoomEnable = HI_TRUE;
		stVoChnAttr.bDeflicker = (0 == nVoDev) ? HI_FALSE : HI_TRUE;
		DVR_ASSERT(HI_MPI_VO_SetChnAttr(nVoDev, i, &stVoChnAttr));
		if(VO_CHN_IS_ONTOP(nDiv, nPage, i)){
			++nLocate;
			DVR_ASSERT(HI_MPI_VO_ChnShow(nVoDev, i));
		}else{
			DVR_ASSERT(HI_MPI_VO_ChnHide(nVoDev, i));
		}
	}
}

static void video_OutputInit()
{
	int i = 0, ii = 0;
	int const nMaxFps = MAX_D1_CNT ? MAX_D1_LIVE_FPS : MAX_CIF_LIVE_FPS;

	VO_VIDEO_LAYER_ATTR_S stVgaLayerAttr;
	VO_VIDEO_LAYER_ATTR_S stCvbsLayerAttr;

	// vga output
	{
		VGA_RESOLUTION const enRes = g_pstSysEnv->GetVGAResolution(g_pstSysEnv);

		stVgaLayerAttr.stImageSize.u32Width  = VO_ORIGIN_WIDTH;
		stVgaLayerAttr.stImageSize.u32Height = VO_ORIGIN_HEIGHT;
		stVgaLayerAttr.stDispRect.s32X = 0;
		stVgaLayerAttr.stDispRect.s32Y = 0;
		switch(enRes) {
		case VGA_800x600:
			stVgaLayerAttr.stDispRect.u32Width  = 800;
			stVgaLayerAttr.stDispRect.u32Height = 600;
			break;
		default:
		case VGA_1024x768:
			stVgaLayerAttr.stDispRect.u32Width  = 1024;
			stVgaLayerAttr.stDispRect.u32Height = 768;
			break;
		case VGA_1280x1024:
			stVgaLayerAttr.stDispRect.u32Width  = 1280;
			stVgaLayerAttr.stDispRect.u32Height = 1024;
			break;
		case VGA_1366x768:
			stVgaLayerAttr.stDispRect.u32Width  = 1366;
			stVgaLayerAttr.stDispRect.u32Height = 768;
			break;
		case VGA_1440x900:
			stVgaLayerAttr.stDispRect.u32Width  = 1440;
			stVgaLayerAttr.stDispRect.u32Height = 900;
			break;
		}

		stVgaLayerAttr.u32DispFrmRt = nMaxFps;
		stVgaLayerAttr.enPixFormat  = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stVgaLayerAttr.s32PiPChn    = VO_DEFAULT_CHN;
	}
	// cvbs
	{
		stCvbsLayerAttr.stImageSize.u32Width  = VO_ORIGIN_WIDTH;
		stCvbsLayerAttr.stImageSize.u32Height = VO_ORIGIN_HEIGHT;
		stCvbsLayerAttr.stDispRect.s32X       = 0;
		stCvbsLayerAttr.stDispRect.s32Y       = 0;
		stCvbsLayerAttr.stDispRect.u32Width   = VO_ORIGIN_WIDTH;
		stCvbsLayerAttr.stDispRect.u32Height  = VO_ORIGIN_HEIGHT;
		stCvbsLayerAttr.u32DispFrmRt          = nMaxFps;
		stCvbsLayerAttr.enPixFormat           = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		stCvbsLayerAttr.s32PiPChn             = VO_DEFAULT_CHN;
	}
	{
		// configuration active
		const VO_VIDEO_LAYER_ATTR_S astVideoLayerAttr[] = {stVgaLayerAttr, stCvbsLayerAttr};

        video_ScreenInit();

		for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
			//DVR_ASSERT(HI_MPI_VO_SetDispBufLen(nDev, MAX_CAM_CH));
			DVR_ASSERT(HI_MPI_VO_SetVideoLayerAttr(VO_DEV_ID[i], &astVideoLayerAttr[i]));
			DVR_ASSERT(HI_MPI_VO_EnableVideoLayer(VO_DEV_ID[i]));
			// setup division
			video_Division(VO_DEV_ID[i], s_stVideo.nScrWidth[i], s_stVideo.nScrHeight[i], -1, 0, FALSE);
			// enable vo channel
			for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
				DVR_ASSERT(HI_MPI_VO_SetChnField(VO_DEV_ID[i], ii, VO_FIELD_BOTTOM));
				DVR_ASSERT(HI_MPI_VO_EnableChn(VO_DEV_ID[i], ii));
				if(ii < MAX_CAM_CH){
					video_BindOutput(VO_DEV_ID[i], ii, TRUE);
				}else{
					DVR_ASSERT(HI_MPI_VO_SENDFRAME(VO_DEV_ID[i], ii, s_pstLogoPic));
				}
			}
		}
	}
	s_stVideo.bOutput = TRUE;
}

static void video_OutputDestroy()
{
	int i = 0, ii = 0;
	for(i = 0; i < sizeof(VO_DEV_ID) / sizeof(VO_DEV_ID[0]); ++i){
		// enable vo channel
		for(ii = 0; ii < VIDEO_GetMaxDiv(); ++ii){
			DVR_ASSERT(HI_MPI_VO_DisableChn(VO_DEV_ID[i], ii));
		}
		// disable video layer
		DVR_ASSERT(HI_MPI_VO_DisableVideoLayer(VO_DEV_ID[i]));
	}
	video_ScreenExit();
	s_stVideo.bOutput = FALSE;
}

int video_DigitalZoom(int nChn, int nRatioX, int nRatioY, int nRatioW, int nRatioH, unsigned char bLive)
{
	return -1;
}

static void video_ClearDigitalZoom(int nChn)
{
}

