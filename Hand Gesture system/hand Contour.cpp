#include <cv.h>
#include <highgui.h>
#include <stdio.h>

    
    CvCapture *capture;
    
    IplImage *frame; //��v���D�e�� 
    IplImage* mask = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1); //�G�ȤƼv�� for ������� 
    IplImage *frame2 = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);//�����⤧�v�� 
    
    CvMemStorage* storage = cvCreateMemStorage(0); //�����x�s�Ŷ� 
    CvMemStorage* defectstorage = cvCreateMemStorage(0);//�ʳ��x�s�Ŷ� 
    CvMemStorage* palmstorage = cvCreateMemStorage(0);//�x�߶��I�x�s�Ŷ�       
    CvMemStorage* fingerstorage = cvCreateMemStorage(0);//���y�x�s�Ŷ� 
    CvSeq* fingerseq = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),fingerstorage);//���y�ǦC 
    
    float radius;//�̤p�]���b�| 
    CvPoint2D32f mincirclecenter;//�̤p�]��ꤤ�� 
    CvPoint mincirclecenter2;//�̤p�]��ꤤ����int 
    	  
    CvSeq* contours;//���� 
    CvSeq* hull;//�Y�] 
    CvSeq* defect;//�ʳ� 
    CvSeq* palm = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),palmstorage); //�x�s���I���ǦC
    //int hullcount;//hull�O�� 
    //int palmcount;
    CvPoint pt0,pt,p,armcenter;//center:�x���I  armcenter:���������I 
    CvBox2D palmcenter,contourcenter; //�x�ߤ��I, ���������I 
      	
	void getconvexhull();//���o�Y�]�P�ʳ� 
	void fingertip();//��M���y 
	void hand();//�e�� 
	
	bool savepic = false;
	
	int palmsize[5];//�s���x�j�p�ǦC
    int palmsizecount=0; 
    bool palmcountfull = false;
    
    CvPoint palmposition[5];//�s���x�j�p�ǦC
    int palmpositioncount=0; 
    bool palmpositionfull = false;
	
    //char display[10];

    



int main()
{             
    CvVideoWriter *writer;
    
    //capture =cvCreateFileCapture("hand4.avi") ;//�q�ɮ�Ū���v�� 
    //
    capture = cvCaptureFromCAM(0) ; //�άO�q��v�� 
    cvNamedWindow("Webcam",0); //�v���ε��� 
    //cvNamedWindow("Virtual hand",0); //�e��ε��� 
    writer = cvCreateVideoWriter("palm_output2.avi",CV_FOURCC('M','J','P','G'),15,cvSize(640,480),1);
          
    while(1) //Ū���v�����^�� 
    {
        frame = cvQueryFrame(capture);//Ū�@�e�� 
        //cvWriteFrame(writer,frame);
        cvCvtColor(frame,frame,CV_BGR2HSV); //��hsv 
        
      for(int i=0;i<frame->height;i++) //�G�Ȥ� , ��HSV��V�Ȥ���0~100���� 
        {
	        for(int j=0;j<frame->width;j++)
            {
		       //if(frame->imageData[i*frame->widthStep+(j*3)+2] < 90 && frame->imageData[i*frame->widthStep+(j*3)+2] > 0 && frame->imageData[i*frame->widthStep+(j*3)+1] < 0) 
               if(frame->imageData[i*frame->widthStep+(j*3)] < 50 || frame->imageData[i*frame->widthStep+(j*3)+2] > 170) 
               { mask->imageData[i*mask->width+j] = 255;}
		       else mask->imageData[i*mask->width+j] = 0;
            }
         } //�G�ȤƵ���  
        
        cvCvtColor(frame,frame,CV_HSV2BGR);//��^BGR 
        cvCopy(frame,frame2);
        //cvErode(mask,mask,0,2); //�I�k 
        
        cvErode(mask,mask,0,1);
        cvDilate(mask,mask,0,1);  //���� 
                       
        /////////////////////////////////////////////////////////////////////////////////////////////////////// ���Ϭq��X����
        cvFindContours( mask, storage, &contours, sizeof(CvContour),                                         // �åBø�s�j��650���I������
                   CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );                                   //
                                                                                                             //  
        while(contours && contours->total <= 650)                                                            // 
        {                                                                                                    //
		      contours = contours->h_next;                                                                   //
        }                                                                                                    //  
	                                                                                                         //    
	    cvDrawContours( frame, contours, CV_RGB(100,100,100), CV_RGB(0,255,0), 1, 2, CV_AA, cvPoint(0,0) );      //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////    
                                                                                                                  
                                                                                                                  
        /////////////////////////////////////////////////////////////////////////////////////////////////////// �}�l�@�~
        if(contours)//�p�G�������~���� 
        {
               //////////////////////////////////////////////////////////////////�p���ӽ��������I 
	             contourcenter =  cvMinAreaRect2(contours,0);
                 armcenter.x = cvRound(contourcenter.center.x);
                 armcenter.y = cvRound(contourcenter.center.y);
                 //cvCircle(frame,armcenter,10,CV_RGB(255,255,255),-1,8,0);
                 //////////////////////////////////////////////////////////////////
                 
                 /////////////////////////////////////////////////////////////////
                 getconvexhull(); //���o�Y�]��T
                 fingertip();//������y 
                 hand();//ø���T                                                                                                  
        }//if������ 
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////        
                                                                                                                
        cvShowImage("Webcam",frame); //���frame or mask
        
        //cvShowImage("Virtual hand",virtualhand);//��ܵe�� 
        
        if(savepic)
        {
           int framenum = (int)cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);
           char name[10];
           itoa(framenum,name,10);
           sprintf(name,"%sfix4.jpg",name);
           //printf("%s\n",name);
           //cvSaveImage(name,frame);
           savepic = false;           
        }
        
        //printf("FPS:%d\n",(int)cvGetCaptureProperty(capture,CV_CAP_PROP_FPS));

       // cvZero(virtualhand);//�N�e��M�� 
        
        if(cvWaitKey(1)>=0 || !frame)
        {
              //cvSaveImage("normal.jpg",frame2);
              break;
        }
    }       
    cvReleaseCapture(&capture);
    cvDestroyWindow("Webcam");
    //cvDestroyWindow("Virtual hand");
    cvReleaseVideoWriter(&writer);
}


////////////////////////////////////////////////////////

void getconvexhull()
{
                    hull = cvConvexHull2( contours, 0, CV_CLOCKWISE, 0 );
                    pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
        
                    for(int i = 0; i < hull->total; i++ )
                    {
                            pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
			                //printf("%d,%d\n",pt.x,pt.y);
                            cvLine( frame, pt0, pt, CV_RGB( 128, 128, 128 ),2,8,0);
                            pt0 = pt;
                    }
                    
                    ////////////////////////////////////////////////////////////////////////
                    
                    defect = cvConvexityDefects(contours,hull,defectstorage); //��M�ʳ� 
                    
                    for(int i=0;i<defect->total;i++)
                    {
			             CvConvexityDefect* d=(CvConvexityDefect*)cvGetSeqElem(defect,i);
			
			
		 	            // if(d->depth < 50) //�B�z�`�׶W�L50���r�] 
//                         {
//			                 p.x = d->start->x;
//			                 p.y = d->start->y;
//		                     //cvCircle(frame,p,5,CV_RGB(255,255,255),-1,CV_AA,0);
//                             p.x = d->end->x;
//			                 p.y = d->end->y;
//		                     //cvCircle(frame,p,5,CV_RGB(255,255,255),-1,CV_AA,0);
//			             }
			
			             if(d->depth > 10) //�B�z�j��10�� 
                         {
			                p.x = d->depth_point->x;
		             	    p.y = d->depth_point->y;
		                    cvCircle(frame,p,5,CV_RGB(255,255,0),-1,CV_AA,0);
			                cvSeqPush(palm,&p);
                         }
			
			
                     }

		             //if(palm->total>1) //�Ʀr�M�w�Ѥ��x�ߧP�O���I�Ӽ� 
//                     {
//		                 cvMinEnclosingCircle(palm,&mincirclecenter,&radius);
//		                 //cvRound(radius);
//		                 mincirclecenter2.x = cvRound(mincirclecenter.x);
//		                 mincirclecenter2.y = cvRound(mincirclecenter.y);
//		                 cvCircle(frame,mincirclecenter2,cvRound(radius),CV_RGB(255,128,255),4,8,0);
//		                 cvCircle(frame,mincirclecenter2,10,CV_RGB(255,128,255),4,8,0);
//                         
//                        palmcenter =  cvMinAreaRect2(palm,0);
//		                 center.x = cvRound(palmcenter.center.x);
//		                 center.y = cvRound(palmcenter.center.y);
//		                 cvEllipseBox(frame,palmcenter,CV_RGB(128,128,255),2,CV_AA,0);
//		                cvCircle(frame,center,10,CV_RGB(128,128,255),-1,8,0);
//
//                     }
				
                          
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void fingertip()
{    
    int dotproduct,i;
	float length1,length2,angle,minangle,length;
	CvPoint vector1,vector2,min,minp1,minp2;
	CvPoint fingertip[20];
	CvPoint *p1,*p2,*p;
	int tiplocation[20];
	int count = 0;
	bool signal = false;

     for(i=0;i<contours->total;i++)
     {
         p1 = (CvPoint*)cvGetSeqElem(contours,i);
		 p = (CvPoint*)cvGetSeqElem(contours,(i+20)%contours->total);
		 p2 = (CvPoint*)cvGetSeqElem(contours,(i+40)%contours->total);
		 vector1.x = p->x - p1->x;
		 vector1.y = p->y - p1->y;
		 vector2.x = p->x - p2->x;
		 vector2.y = p->y - p2->y;
		 dotproduct = (vector1.x*vector2.x) + (vector1.y*vector2.y); 
		 length1 = sqrtf((vector1.x*vector1.x)+(vector1.y*vector1.y));
		 length2 = sqrtf((vector2.x*vector2.x)+(vector2.y*vector2.y));
		 angle = fabs(dotproduct/(length1*length2));    
         
         ///////////////////////////////////if start
         if(angle < 0.2)
         {
            //cvCircle(frame,*p,4,CV_RGB(0,255,255),-1,8,0); //��ܩҦ��I 
            
            if(!signal)//�p�G�O�Ĥ@�I 
            {
				signal = true;
				min.x = p->x;
				min.y = p->y;
				minp1.x = p1->x;
				minp1.y = p1->y;
				minp2.x = p2->x;
				minp2.y = p2->y;
				minangle = angle;
			}      
			else//�p�G���O�Ĥ@�I 
            {
				if(angle <= minangle)//�p�Gangle�ȧ�p 
                {
					
					min.x = p->x;
					min.y = p->y;
					minp1.x = p1->x;
					minp1.y = p1->y;
					minp2.x = p2->x;
					minp2.y = p2->y;
					minangle = angle;
				}
			}
						
	     }//////////////////////////////////////////////////////////if end   
	     
	     
	     else//else start
	     {
             if(signal)
             {
                signal = false;
                CvPoint l1,l2,l3;//temp1�����ߨ���y, vector1�����y��p1, vector2�����y��p2
				l1.x = min.x - armcenter.x;
				l1.y = min.y - armcenter.y;
				
				l2.x = minp1.x - armcenter.x;
				l2.y = minp1.y - armcenter.y;

				l3.x = minp2.x - armcenter.x;
				l3.y = minp2.y - armcenter.y;
				
				length = sqrtf((l1.x*l1.x)+(l1.y*l1.y));
				length1 = sqrtf((l2.x*l2.x)+(l2.y*l2.y));
				length2 = sqrtf((l3.x*l3.x)+(l3.y*l3.y));    
                
                if(length > length1 && length > length2)
                {
					//cvCircle(frame,min,6,CV_RGB(0,255,0),-1,8,0);
					fingertip[count] = min;
				    tiplocation[count] = i+20;
				    count = count + 1;
				}
				
				else if(length < length1 && length < length2)
                {
					//cvCircle(frame,min,8,CV_RGB(0,0,255),-1,8,0);
					//cvCircle(virtualhand,min,8,CV_RGB(255,255,255),-1,8,0);
					cvSeqPush(palm,&min);
					//fingertip[count] = min;
				    //tiplocation[count] = i+20;
				    //count = count + 1;
				}
				
                      
             }
         }//else end
		
		
      }//for end	
      
      
	  
	  for(i=0;i<count;i++)
      {
		if( (tiplocation[i] - tiplocation[i-1]) > 40)
        {
		  if( fingertip[i].x >= 630  || fingertip[i].y >= 470 )
		  {
             cvCircle(frame,fingertip[i],6,CV_RGB(50,200,250),-1,8,0);                           
          }
          
          else
          {
          //cvCircle(frame,fingertip[i],6,CV_RGB(0,255,0),-1,8,0);
		//cvCircle(virtualhand,fingertip[i],6,CV_RGB(0,255,0),-1,8,0);
		//cvLine(virtualhand,fingertip[i],armcenter,CV_RGB(255,0,0),3,CV_AA,0);
		  cvSeqPush(fingerseq,&fingertip[i]);
          }
		}
	  }
	    //cvClearSeq(fingerseq);    
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void hand()
{
     bool useavepalm = true;

     if(palm->total <= 2)
     {
        useavepalm = false;            
        CvFont Font1=cvFont(3,3);
        cvPutText(frame,"Error Palm Position!!",cvPoint(10,50),&Font1,CV_RGB(255,0,0));  
        //savepic = true;
        
        CvPoint *temp,*additional,*palmtemp;
        CvMemStorage* palm2storage = cvCreateMemStorage(0);
        CvSeq* palm2 = cvCreateSeq(CV_SEQ_ELTYPE_POINT,sizeof(CvSeq),sizeof(CvPoint),palm2storage); //�x�s���I���ǦC     
        
        for(int i=0;i<palm->total;i++)
        {
           palmtemp = (CvPoint*)cvGetSeqElem(palm,i);
                
           for(int j=1;j<contours->total;j++)
           {
             temp =  (CvPoint*)cvGetSeqElem(contours,j);  
             if(temp->y == palmtemp->y && temp->x == palmtemp->x)
             {
                additional = (CvPoint*)cvGetSeqElem(contours,(int)(j+((contours->total)/2))%(contours->total));
                if(additional->y <= palmtemp->y)
                cvCircle(frame,*additional,10,CV_RGB(0,0,255),-1,8,0); 
                cvSeqPush(palm2,additional);                  
             }                                
           }             
        }
        
        for(int i=0;i<palm2->total;i++)
        {
           temp = (CvPoint*)cvGetSeqElem(palm2,i); 
           cvSeqPush(palm,temp);    
        }
        
        for(int i=1;i<contours->total;i++)
        {
             temp =  (CvPoint*)cvGetSeqElem(contours,1);  
             if(temp->y <= additional->y)
             additional = temp;        
        }
        cvCircle(frame,*additional,10,CV_RGB(0,0,255),-1,8,0);
        cvSeqPush(palm,additional);
                                 
     }
    
     //////////////////////////////////////////////////////////////////////////////��Υ�����m 
     cvMinEnclosingCircle(palm,&mincirclecenter,&radius);
     mincirclecenter2.x = cvRound(mincirclecenter.x);
     mincirclecenter2.y = cvRound(mincirclecenter.y);
    
    if(useavepalm){
    CvPoint avePalmCenter,distemp;
    int lengthtemp,radius2;
    avePalmCenter.x = 0;
    avePalmCenter.y = 0;
    
    for(int i=0;i<palm->total;i++) // �p�⥭����m 
    {
            CvPoint *temp = (CvPoint*)cvGetSeqElem(palm,i); 
            avePalmCenter.x += temp->x;
            avePalmCenter.y += temp->y;        
    }
     
     avePalmCenter.x = (int)(avePalmCenter.x/palm->total);
     avePalmCenter.y = (int)(avePalmCenter.y/palm->total);
     radius2 = 0;
     
     for(int i=0;i<palm->total;i++) //�p�⥭���Z�� 
    {
            CvPoint *temp = (CvPoint*)cvGetSeqElem(palm,i); 
            distemp.x = temp->x - avePalmCenter.x;      
            distemp.y = temp->y - avePalmCenter.y;  
            lengthtemp =  sqrtf(( distemp.x* distemp.x)+(distemp.y*distemp.y));
            radius2 += lengthtemp;
    }
    
    radius2 = (int)(radius2/palm->total);
    radius = ((0.5)*radius + (0.5)*radius2);
    mincirclecenter2.x =  ((0.5)*mincirclecenter2.x + (0.5)*avePalmCenter.x);
    mincirclecenter2.y =  ((0.5)*mincirclecenter2.y + (0.5)*avePalmCenter.y);
}
     //////////////////////////////////////////////////////////////////////////////////////���x�ߦ�m����
     
     palmposition[palmpositioncount].x = cvRound(mincirclecenter2.x);
     palmposition[palmpositioncount].y = cvRound(mincirclecenter2.y);
     palmpositioncount = (palmpositioncount+1)%3;
     
     if(palmpositionfull)
     {
        float xtemp=0,ytemp=0;
        for(int i=0;i<3;i++)
        {
           xtemp += palmposition[i].x;   
           ytemp += palmposition[i].y;          
        }        
        
        mincirclecenter2.x = cvRound(xtemp/3); 
        mincirclecenter2.y = cvRound(ytemp/3);    
     }
     
     if(palmpositioncount == 2 && palmpositionfull == false)
     {
        palmpositionfull = true;                       
     }
     
     cvCircle(frame,mincirclecenter2,10,CV_RGB(0,255,255),4,8,0);
     //cvCircle(virtualhand,mincirclecenter2,10,CV_RGB(0,255,255),4,8,0);
     
   
     
     ///////////////////////////////////////////////////////////////////////////////////////����x�j�p������ 
     
     palmsize[palmsizecount] = cvRound(radius);
     palmsizecount = (palmsizecount+1)%3;
     
     if(palmcountfull)
     {
        float tempcount=0;
        for(int i=0;i<3;i++)
        {
           tempcount += palmsize[i];             
        }        
        
        radius = tempcount/3;     
     }
     
     if(palmsizecount == 2 && palmcountfull == false)
     {
        palmcountfull = true;                       
     }
     
    cvCircle(frame,mincirclecenter2,cvRound(radius),CV_RGB(255,0,0),2,8,0);
    cvCircle(frame,mincirclecenter2,cvRound(radius*1.2),CV_RGB(200,100,200),1,8,0);
    //cvCircle(virtualhand,mincirclecenter2,cvRound(radius),CV_RGB(255,0,0),2,8,0);
    //cvCircle(virtualhand,mincirclecenter2,cvRound(radius*1.3),CV_RGB(200,100,200),1,8,0);
     
     //////////////////////////////////////////////////////////////////////////////////////
    
    int fingercount = 0; 
    float fingerlength;
    CvPoint tiplength,*point;
     
    for(int i=0;i<fingerseq->total;i++)
     {
         point = (CvPoint*)cvGetSeqElem(fingerseq,i);
         tiplength.x = point->x - mincirclecenter2.x;
         tiplength.y = point->y - mincirclecenter2.y;
         fingerlength = sqrtf(( tiplength.x* tiplength.x)+(tiplength.y*tiplength.y));
         
         if((int)fingerlength > cvRound(radius*1.2))
         {
            fingercount += 1;   
            cvCircle(frame,*point,6,CV_RGB(0,255,0),-1,8,0);
                       
         }        
     }
     
 

     
     
     
     
     cvClearSeq(fingerseq); 
     cvClearSeq(palm);
}
