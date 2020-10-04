#pragma warning(disable:4996)


#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TRender.h"

//example:
//convert("C:\\Users\\mbra\\IdeaProjects\\SolarSystemSimulator-master\\Models\\17244_Voyager_2_v1_NEW2.obj", "C:\\Users\\mbra\\IdeaProjects\\SolarSystemSimulator-master\\Models\\voyager2_out.obj" );

// ad-hoc conversion of ascii/plain model files to .obj
void convert(char* in_filename, char* out_filename)
{
	int nv = 0;
	int nf = 0;
	FILE *f, *g;
	float x,y,z;
	char buf2[10];
	if ((f=fopen(in_filename, "r"))!=NULL)
	{
		g=fopen(out_filename,"w");
		while (!feof(f))
		{
			fscanf(f, "%s", &buf2[0]);
			if (!strcmp(buf2, "vt"))
			{
				float x,y;
				fscanf(f, "%f %f", &x, &y, &z);
			}
			if (!strcmp(buf2, "vn"))
			{
				float x,y,z;
				fscanf(f, "%f %f %f", &x, &y, &z);
			}
			else if (!strcmp(buf2, "v"))
			{
				fscanf(f, "%f %f %f", &x, &y, &z);
				fprintf(g, "%f %f %f\n", x,y,z);
				nv++;
			}	
			else if (!strcmp(buf2, "f"))
			{
				int v0,n0,v1,n1,v2,n2,v3,n3;
				int t0,t1,t2,t3;
				v3=-1;t3=-1;n3=-1;
				fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v0,&t0,&n0,&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3);
				if (v3==-1) 
				{
					//printf("[] %d %d %d\n", v0,v1,v2);
					fprintf(g,"3 %d %d %d\n", v0, v1, v2);
					nf++;
				}
				else 
				{
					fprintf(g,"3 %d %d %d\n", v0, v1, v2);
					fprintf(g,"3 %d %d %d\n", v0, v2, v3);
					nf+=2;
				}
			}
		}
		fclose(f);
		fclose(g);
		printf("NV is %d\n", nv);
		printf("NF is %d\n", nf);
	}
}


void convert2(char* in_filename, char* out_filename)
{
	int nv = 0;
	int nf = 0;
	FILE *f, *g;
	float x,y,z;
	char buf2[10];
	int v0,v1,v2;
					
	if ((f=fopen(in_filename, "r"))!=NULL)
	{
		g=fopen(out_filename,"w");
		while (!feof(f))
		{
			
			float aFloat;
			fscanf(f, "%s", buf2);
			if (!strcmp(buf2,"c"))
			{
				fscanf(f, "%d %d %d", &v0, &v1, &v2);
			}
			else 
			{
				aFloat = (float) (atof(buf2));
				if (aFloat != 3.000000f)
				{
					x = aFloat;
					fscanf(f, "%f %f", &y, &z);
					fprintf(g, "v %f %f %f\n", x,y,z);
					nv++;
				}
				else 
				{
					fscanf(f, "%d %d %d", &v0, &v1, &v2);
					fprintf(g, "f %d %d %d\n", v0, v1, v2);
					nf++;
				}
			}
		}
		fclose(f);
		fclose(g);
		printf("NV is %d\n", nv);
		printf("NF is %d\n", nf);
	}
}

// 904; theta Oct          ;  0  1 35.702946 ;  -77  3 56.60834 ;   4.78 
void createTextureFromFK6(TRender& myRenderer, char* in_filename, char* out_filename)
{
	FILE* f;
	if ((f=fopen(in_filename,"r"))==NULL) return;
	while (!feof(f))
	{
		int ID;				
		int RA_1;
		int RA_2;
		float RA_3;
		int DE_1;
		int DE_2;
		float DE_3;
		float V;
		fscanf(f, "%d; %d%d%f; %d%d%f; %f", &ID, &RA_1, &RA_2, &RA_3, &DE_1, &DE_2, &DE_3 , &V);
		
		float RA = 4095.0f*(RA_1*3600.0f + RA_2*60.0f + RA_3)/86400.0f;
		float DEC = 1023.0f*(abs(DE_1) + DE_2/(60.0f) + DE_3/(3600.0f))/90.0f;
		if (DE_1<0) DEC=-DEC;

		printf("%d; %d %d %f; %d %d %f; %f (RA=%f, DEC=%f)\n", ID, RA_1, RA_2, RA_3, DE_1, DE_2, DE_3, V, RA, DEC);

		// experimental
		//float r = 2.0*(V-2.0f)*(V-2.0f)/100.0f+0.2f;
		float r = (V-2.0f)/10.0f+0.2f;
		printf ( "r is %f\n", r );
		myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, 1, (int) r*255, (int) r*255, (int) r*225 );
		//myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, 2, r*255, r*255, r*225 );
		//myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, 3, r*200, r*170, r*0 );
		
	}
	fclose(f);
	
}


#define MIN(a,b) ((a)<(b) ? (a) : (b))
void createTextureFromStarCatalogDat(TRender& myRenderer, char* in_filename, char* out_filename)
{
	FILE* f;
	char buf[2048];
	if ((f=fopen(in_filename,"r"))==NULL) return;
	for (int i =0;i<1628;i++)	
	{
		float RA,DEC,V,RA0,DEC0;
		fgets(buf, 500, f);
		sscanf(&buf[73], "%f", &RA0);
		RA=4095.0f*(RA0)/360.0f;
		//printf("RA is %f\n", RA);
		sscanf(&buf[80], "%f", &DEC0);
		DEC=1023.0f*DEC0/90.0f;
		//printf("DEC is %f\n", DEC);
		sscanf(&buf[171], "%f", &V);
		printf("%f\n", V);
		//V=MIN(10*pow(10,-V/2.5),1);		
		//printf("V is %f\n", MIN(10*pow(10,-V/2.5),1));
		// [-1.46/5.03]
		float r = (float) MIN(10.0f*pow(10,-V/2.5),1.0f);		
		r=sqrt(r);
		
		myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, 1, (int) r*255, (int)r*255, (int)r*225 );
		myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, 2, (int)r*255, (int)r*255, (int)r*225 );
		if (V<0.0)
		{
			printf("V negative\n");
			for (int j=3; j<4; j++)
				myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, j, (int)r*255, (int)r*255, (int)r*225 );
		}
		if ((V<-1.4) || ((abs(RA0-165.70)<0.01) && (abs(DEC0-56.38)<0.01)))
		{
			// sirius
			for (int j=3; j<4; j++)
				myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, j, 255, 0, 0 );
		}
		if (((abs(RA0-37.78)<0.01) && (abs(DEC0-46.62)<0.01)))
		{
			for (int j=3; j<4; j++)
				myRenderer.drawCircle( (int) RA, 1023 + (int) DEC, j, 0, 0, 255 );
		}
		  
	}
	fclose(f);
}

