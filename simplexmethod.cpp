#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>
#define NMAX 500
#define EMAX 500
#define EPSILON 10.0e-2
using namespace std;
int read_problem(int *n,int *m,float obj[NMAX][EMAX]);//read the from input file.
bool initial_dictionary(int n,int m,float obj[NMAX][EMAX]);
int solvetableau(int n,int m,float obj[NMAX][EMAX],int *basis);
int solvetableau1(int n,int m,float obj[NMAX][EMAX],int *basis);
int select_column(int n,int m,float obj[NMAX][EMAX]);	
int select_row(int n,int m,float obj[NMAX][EMAX],int enter_var);
int* add_slack(int n,int m,float obj[NMAX][EMAX],bool check_feas);
int auxiliary(int n,int m,float obj[NMAX][EMAX]);
int createfeasible(int n,int m,float obj[NMAX][EMAX],int *basis);
int calculateobj(int n,int m,float obj[NMAX][EMAX],int *basis);
			
int main(int argc,char *argv[])
{
	int n,m;
	int *basis;
	float obj[NMAX][EMAX];
	if (argc != 2)
    {
    	printf("Usage %s <pivot_rule>\n", argv[0]);
    	printf("Pivot Rule:\n");
    	printf("1 - smallest subscript.\n");
    	exit(0);
    }
    int pivot_rule=atoi(argv[1]);
    
	while(read_problem(&n,&m,obj))
	{
		bool check_feas = initial_dictionary(n,m,obj);
		//add_slack(n,m,obj);
		/*printf("n=%d m=%d \n",n,m);
		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
    */
	 
		if(check_feas==false)
		{
		//TO DO: auxiliary dictionary and create initial feasible dictionary
			printf("Infeasible origin: convert it to auxiliary problem\n");
			auxiliary(n,m,obj);
			basis= add_slack(n,m,obj,check_feas);
			createfeasible(n,m,obj,basis);
			int pivot_col=solvetableau1(n,m,obj,basis);
		
			if(pivot_col!=-2)
			{
			
			do{
			//	printf("print");
				//return;
				pivot_col=solvetableau1(n,m,obj,basis);
				
				if(pivot_col==-2)
				break;
				
				}while(pivot_col!=-1);
			
				if(pivot_col!=-2)
				{
				
				calculateobj(n,m,obj,basis);			
			
				pivot_col=solvetableau(n,m,obj,basis);
				
				if(pivot_col!=-2)
				{
				
					do{
				//	printf("print");
					//return;
					pivot_col=solvetableau(n,m,obj,basis);
					if(pivot_col==-2)
					break;
				//	printf("PC=%d",pivot_col);
					}while(pivot_col!=-1);
				}
				}
			}
			
		
		}
		else
		{
			
			basis= add_slack(n,m,obj,check_feas);
			/*
			for ( int i = 1; i <=m; i++ ) {
      			printf( "*(basis + %d) : %d\n", i, *(basis+i));
   			}*/
			int pivot_col=solvetableau(n,m,obj,basis);
			// solvetableau(n,m,obj,basis);
		if(pivot_col!=-2)
		{
		
			do{
		//		printf("print");
				//return;
				pivot_col=solvetableau(n,m,obj,basis);
				if(pivot_col==-2)
				break;
			}while(pivot_col!=-1);
		}
		}
		
	
	}
	
	

}

int read_problem(int *n,int *m,float obj[NMAX][EMAX])//read the from input file.
{


	int i,j;
	if(scanf("%d",n)==-1)
	{
	
		return 0;//retuned when file empty
	
	}scanf("%d",m);
	//printf("N=%d....M=%d",*n,*m);
	
	for(i=0;i<(*m+1);i++)
	{	
		for(j=0;j<(*n+*m+1);j++)
		{
			if(i==0 && j==0)
			{
				obj[i][j]=0.0;
			}
			else if(j>=(*n+1))
			{
				if(j==(*n+i))
				obj[i][j]=1;
				else
				obj[i][j]=0;
				
				if(i==0)
				obj[i][j]=0;
				
				
				//i++;		
			}
			else
			scanf("%f",&obj[i][j]);
			
			
		}
		
    }
    

}

bool initial_dictionary(int n,int m,float obj[NMAX][EMAX])
{
			bool feasible = true;
			printf("\n=============================================================================================\nGiven Problem: \n Maximize\t");
	
			for(int j=1;j<=(n);j++)
			{
				if(obj[0][j]>0)
					printf(" + %f X%d ",obj[0][j],j);
					else if(obj[0][j]==0)
					printf(" + %f X%d ",obj[0][j],j);
					else
					printf(" %f X%d ",obj[0][j],j);
			}
	
			printf("\nsubject to: \n");
			for(int i=1;i<(m+1);i++)
			{	
				for(int j=1;j<=(n);j++)
				{
					if(obj[i][j]>0)
					printf(" + %f X%d ",obj[i][j],j);
					else if(obj[i][j]==0)
					printf(" + %f X%d ",obj[i][j],j);
					else
					printf(" %f X%d ",obj[i][j],j);
				}
				printf("<= %f\n",obj[i][0]);
				if(obj[i][0]<0)
				feasible=false;
	    	}
	    
	return feasible;
}

int solvetableau(int n,int m,float obj[NMAX][EMAX],int *basis)
{
		float multiple=0;
		int leave_var;
		float tempobj[NMAX][EMAX];
		for(int i=0;i<=m;i++)
		{
			for(int j=0;j<n+m+1;j++)
			{
				tempobj[i][j]=0;
			}
		}
		int pivot_col = select_column(n,m,obj);	
		
		if(pivot_col==-1)
		{
				return pivot_col;
		}
		printf("\nentering variable is X%d",pivot_col);
		
		int pivot_row = select_row(n,m,obj,pivot_col);
		
		if(pivot_row==-2)
    	{
    		printf("\nOOPS! nothing to leave \n problem is unbounded \n");
    		
    		return pivot_row;
    	}

		
		
			for ( int i = 1; i <=m; i++ ) {
				if(i==pivot_row)	
      				leave_var=*(basis+i);
				  //printf( "*(basis + %d) : %d\n", i, *(basis+i));
   			}
			printf("	leaving variable is X%d \n",leave_var);
		
			for ( int i = 1; i <=m; i++ ) {
				if(basis[i]==leave_var)
				{
					
					basis[i]=pivot_col;
					
				}	
      				
				  //printf( "*(basis + %d) : %d\n", i, basis[i]);
   			}
		
		
		
		float pivot_number =obj[pivot_row][pivot_col];
	//		printf(" pivot row =%d  pivot col=%d\n",pivot_row,pivot_col);
	//		printf(" pivot number =%f\n",pivot_number);
	
		for(int j=0; j<n+m+1;j++)
		{
			tempobj[pivot_row][j]= obj[pivot_row][j]/pivot_number;
		}
		
		
		for(int i=0;i<=m;i++)
		{
		
			for(int j=0;j<n+m+1;j++)
			{
				if(i!=pivot_row)	
				{
					
						multiple = obj[i][pivot_col]/obj[pivot_row][pivot_col];	
		//				printf("\n i=%d j=%d --m=%f --\n",i,j,multiple);
						tempobj[i][j] = obj[i][j]- obj[pivot_row][j] * multiple;
					
				}		
				//obj[i][j]= obj[i][j]-obj[pivot_row][j]*( obj[i][j]/obj[pivot_row][j]);
		//	printf("%f ",obj[i][j]);
				
			}
			//printf("\n");
		}
		/*
		printf ("\nROW\n");
		for(int j=0; j<=n+m+1;j++)
		{
			printf("%f\t",obj[pivot_row][j]); 
		}
		*/
		for(int i=0;i<=m;i++)
		{
		
			for(int j=0;j<n+m+1;j++)
			{
		
				obj[i][j]=tempobj[i][j];
		
		
			}	
		}
	/*	
		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
	*/	
		//printf("n=%d m=%d \n",n,m);
		for(int i=0;i<(m+1);i++)
		{	
			if(i!=0)
			printf(" X%d = ",basis[i]);
				
			for(int j=0;j<(n+m+1);j++)
			{
				if(j==0)
				{
					if(i==0)
					{	//float tem= -(obj[i][j]);
						//printf("z= %f ",tem);
					}
					else
					printf(" %f ",obj[i][j]);
				}
				else if(i==0 && j!=0)
				{
					
				}	
				
				else if(j==basis[i])
				{
					
				}
				else if(i!=0 )
			//	printf(" %fX%d ",obj[i][j],j);
			//	else 
				{
						if(obj[i][j]>0)
						printf(" - %fX%d ",obj[i][j],j);	
						else if(obj[i][j]<0)
						{
							float tem= -(obj[i][j]);
							printf(" + %fX%d ",tem,j);	
						}
						else if(obj[i][j]==0)
						printf(" + %fX%d ",obj[i][j],j);
				}
				
				
			}
			printf("\n");
    	}
    
	 	float tem= -(obj[0][0]);
	 	printf("-----------------------------------------------------------------------------\n");
						printf("z= %f ",tem);
    		for(int j=1;j<n+m+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
			printf("\n");
	
	
		pivot_col = select_column(n,m,obj);
	 return pivot_col;		
	

	
}

int select_column(int n,int m,float obj[NMAX][EMAX])
{
			float positive_smallsub=0;
			int pivot_col=-1;
			for(int j=1;j<=(n+m);j++)
			{
				//printf("pivot %d",pivot_col);
				if(obj[0][j]>EPSILON && obj[0][j]>positive_smallsub)
				{
					positive_smallsub=obj[0][j];
					pivot_col=j;
					
					//break;
				}
						
			}
			/*if(pivot_col==-1)
			{
				
				printf("The optimal solution z =%f", obj[0][0]);
				exit(0);
			}*/
	return pivot_col;
}
int select_row(int n,int m,float obj[NMAX][EMAX],int enter_var)
{
		float min=1000;
		int pivot_row=-2;
	//	printf("selcted colum\n");
		for(int i=1;i<(m+1);i++)
		{	
	
		//	printf("%f  %f \n",obj[i][0],obj[i][enter_var]);
			
			 if(obj[i][enter_var]>EPSILON && obj[i][0]/obj[i][enter_var]<min)
			 {
			 	
			 	min= obj[i][0]/obj[i][enter_var];
			 	
				pivot_row= i;
			 }
			
    	}
    	    
    return pivot_row;
	 
	
	
	
	
}
int* add_slack(int n,int m,float obj[NMAX][EMAX],bool check_feas)
{
		int* basis = (int *)malloc(sizeof(int) * m);
		
	
		printf("\nInitial dictionary\n");
			for(int i=1;i<m+1;i++)
			{
				basis[i]=n+i;
			}
	
			for(int i=1;i<(m+1);i++)
			{	
				printf("X%d = ",basis[i]);
				if(obj[i][0]>0)
					printf(" + %f ",obj[i][0]);
					else if(obj[i][0]==0)
					printf(" + %f ",obj[i][0]);
					else
					printf(" %f ",obj[i][0]);
				
				for(int j=1;j<=(n);j++)
				{
					if(obj[i][j]<0)
					printf(" + %f X%d ",-obj[i][j],j);
					else if(obj[i][j]==0)
					printf(" + %f X%d ",obj[i][j],j);
					else
					printf(" - %f X%d ",obj[i][j],j);
				}
				if(check_feas==false)
				printf(" + X%d ",n+m+1);
				
				printf("\n");
	    	}
	    	if(check_feas==false)
	    	{
		    	printf("-----------------------------------------------------------------------------\n");
				printf("Z = -X%d \n",n+m+1);		
				
			}else
			{
				float tem= -(obj[0][0]);
			 	printf("-----------------------------------------------------------------------------\n");
						printf("z= %f ",tem);
    		for(int j=1;j<n+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
				printf("\n");
			}
	return basis;
}
int auxiliary(int n,int m,float obj[NMAX][EMAX])
{
		printf("\nGiven Problem: \n Maximize\t  - X%d \n ",n+m+1);
	
			for(int i=0;i<(m+1);i++)
			{
				obj[i][n+m+1]=-1;
				
				
			}
			
			for(int j=0;j<(n+m+1);j++)
			{
				obj[m+1][j]=obj[0][j];
				if(j==n+m+1)
				obj[0][j]=-1;
				else
				obj[0][j]=0;
			}
/*		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}  */
		printf("\nsubject to: \n");
			for(int i=1;i<(m+1);i++)
			{	
				for(int j=1;j<=(n);j++)
				{
					if(obj[i][j]>0)
					printf(" + %f X%d ",obj[i][j],j);
					else if(obj[i][j]==0)
					printf(" + %f X%d ",obj[i][j],j);
					else
					printf(" %f X%d ",obj[i][j],j);
				}
				printf(" - X%d <= %f\n",n+m+1,obj[i][0]);
			}
	    
	
}

int createfeasible(int n,int m,float obj[NMAX][EMAX],int *basis)
{
	
	/*
		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<=(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
	*/

	int i,pivot_row,pivot_col,multiple;
	float small=0, tempobj[NMAX][EMAX];
	/*for(i=1;i<=n;i++)
	{
		printf("%d\n",basis[i]);
		
		
	}
	*/
	for(i=1;i<=m;i++)
	{
		if(obj[i][0]<small)
		{
			small=obj[i][0];
			pivot_row=i;
				
//			printf("%f",obj[i][0]);
		}
	}
	
	pivot_col=n+m+1;
	
	//printf("here %d",basis[pivot_row]);
	
	printf("\n entering variable is X%d and leaving variable is X%d \n",n+m+1,basis[pivot_row]);
	
	for(i=1;i<=m;i++)
	{
		if(basis[i]==basis[pivot_row])
		{
			basis[i]=n+m+1;
		}
	}
	
	
	
	
	float pivot_number =obj[pivot_row][pivot_col];
	//printf("pivot number = %f\n",pivot_number);
	
	for(int i=0;i<=m+1;i++)
		{
			for(int j=0;j<=n+m+1;j++)
			{
				tempobj[i][j]=0;
			}
		}
		
		for(int j=0; j<=n+m+1;j++)
		{
			tempobj[pivot_row][j]= obj[pivot_row][j]/pivot_number;
		}
		
		
		for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
				if(i!=pivot_row)	
				{
					
						multiple = obj[i][pivot_col]/obj[pivot_row][pivot_col];	
		//				printf("\n i=%d j=%d --m=%f --\n",i,j,multiple);
						tempobj[i][j] = obj[i][j]- obj[pivot_row][j] * multiple;
					
				}		
				//obj[i][j]= obj[i][j]-obj[pivot_row][j]*( obj[i][j]/obj[pivot_row][j]);
		//	printf("%f ",obj[i][j]);
				
			}
		//	printf("\n");
		}
	
	for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
		
				obj[i][j]=tempobj[i][j];
		
		
			}	
		}
		/*
		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
		*/
		//printf("n=%d m=%d \n",n,m);
		for(int i=1;i<(m+1);i++)
		{	
			//if(i!=0)
			printf(" X%d = ",basis[i]);
				
			for(int j=0;j<=(n+m+1);j++)
			{
				if(j==0)
				{
					if(i==0)
					{	
					}
					else
					printf(" %f ",obj[i][j]);
				}
				else if(i==0 && j!=0)
				{
				
				}	
				
				else if(j==basis[i])
				{
					
				}
				else if(i!=0 )
			//	printf(" %fX%d ",obj[i][j],j);
			//	else 
				{
						if(obj[i][j]>0)
						printf(" - %fX%d ",obj[i][j],j);	
						else if(obj[i][j]<0)
						{
							float tem= -(obj[i][j]);
							printf(" + %fX%d ",tem,j);	
						}
						else if(obj[i][j]==0)
						printf(" + %fX%d ",obj[i][j],j);
				}
				
				
			}
			printf("\n");
    	}
    
    		float tem= -(obj[0][0]);
			printf("-----------------------------------------------------------------------------\n");
						printf("z= %f ",tem);
    		for(int j=1;j<=n+m+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
    		
			printf("\n");
}

int solvetableau1(int n,int m,float obj[NMAX][EMAX],int *basis)
{
		float multiple=0;
		int leave_var;
		float tempobj[NMAX][EMAX];
		for(int i=0;i<=m+1;i++)
		{
			for(int j=0;j<=n+m+1;j++)
			{
				tempobj[i][j]=0;
			}
		}
		int pivot_col = select_column(n,m,obj);	
		
		if(pivot_col==-1)
		{
				return pivot_col;
		}
		printf("\nentering variable is X%d",pivot_col);
		
		int pivot_row = select_row(n,m,obj,pivot_col);
		if(pivot_row==-2)
    	{
    		printf("\nOOPS! nothing to leave \n problem is unbounded \n");
    		
    		return pivot_row;
    	}

		
			for ( int i = 1; i <=m; i++ ) {
				if(i==pivot_row)	
      				leave_var=*(basis+i);
				  //printf( "*(basis + %d) : %d\n", i, *(basis+i));
   			}
			printf("	leaving variable is X%d \n",leave_var);
		
			for ( int i = 1; i <=m; i++ ) {
				if(basis[i]==leave_var)
				{
					
					basis[i]=pivot_col;
					
				}	
      				
				  //printf( "*(basis + %d) : %d\n", i, basis[i]);
   			}
		
		
		
		float pivot_number =obj[pivot_row][pivot_col];
		//	printf(" pivot row =%d  pivot col=%d\n",pivot_row,pivot_col);
		//	printf(" pivot number =%f\n",pivot_number);
	
		for(int j=0; j<=n+m+1;j++)
		{
			tempobj[pivot_row][j]= obj[pivot_row][j]/pivot_number;
		}
		
		
		for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
				if(i!=pivot_row)	
				{
					
						multiple = obj[i][pivot_col]/obj[pivot_row][pivot_col];	
		//				printf("\n i=%d j=%d --m=%f --\n",i,j,multiple);
						tempobj[i][j] = obj[i][j]- obj[pivot_row][j] * multiple;
					
				}		
				//obj[i][j]= obj[i][j]-obj[pivot_row][j]*( obj[i][j]/obj[pivot_row][j]);
		//	printf("%f ",obj[i][j]);
				
			}
		//	printf("\n");
		}
		/*
		printf ("\nROW\n");
		for(int j=0; j<=n+m+1;j++)
		{
			printf("%f\t",obj[pivot_row][j]); 
		}
		*/
		for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
		
				obj[i][j]=tempobj[i][j];
		
		
			}	
		}
		/*
		for(int i=0;i<=(m+1);i++)
		{	
			for(int j=0;j<=(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
		*/
		//printf("n=%d m=%d \n",n,m);
		for(int i=0;i<(m+1);i++)
		{	
			if(i!=0)
			printf(" X%d = ",basis[i]);
				
			for(int j=0;j<=(n+m+1);j++)
			{
				if(j==0)
				{
					if(i==0)
					{	//float tem= -(obj[i][j]);
						//printf("z= %f ",tem);
					}
					else
					printf(" %f ",obj[i][j]);
				}
				else if(i==0 && j!=0)
				{
				
				}	
				
				else if(j==basis[i])
				{
					
				}
				else if(i!=0  )
			//	printf(" %fX%d ",obj[i][j],j);
			//	else 
				{
						if(obj[i][j]>0)
						printf(" - %fX%d ",obj[i][j],j);	
						else if(obj[i][j]<0)
						{
							float tem= -(obj[i][j]);
							printf(" + %fX%d ",tem,j);	
						}
						else if(obj[i][j]==0)
						printf(" + %fX%d ",obj[i][j],j);
				}
				
				
			}
			printf("\n");
    	}
    
			float tem= -(obj[0][0]);
			printf("-----------------------------------------------------------------------------\n");
			printf("z= %f ",tem);
    		for(int j=1;j<=n+m+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
 
			printf("\n");
	
		pivot_col = select_column(n,m,obj);
	 	return pivot_col;		
	

	
}
int calculateobj(int n,int m,float obj[NMAX][EMAX],int *basis){
	
	int i,j,pr,pc;
	float tempobj[NMAX][EMAX],multiple;
	bool xinbasis=false;
	for(i=0;i<=n;i++)
	{
		if(basis[i]==n+m+1)
		{
			printf("\nX%d is still in the basis\n\n ");
			printf("leaving variable is X%d",basis[i]);
			
			pr=i;
			xinbasis=true;
		}
		
	}
		
		if(xinbasis==true)
		{
			for(j=0;j<=n+m+1;j++)
			{
			//	printf("obj=%f",obj[pr][j]);
				if(obj[pr][j]<0)
				{
				//	printf("%f",obj[pr][j]);
					pc=j;
					
				//	printf("--%d--",pc);
					break;	
				}
			}
			
			basis[pr]=pc;
			printf(" and entering variable is X%d\n",pc);
				
		float pivot_number =obj[pr][pc];
	//printf("pivot number = %f\n",pivot_number);
	
	for(int i=0;i<=m+1;i++)
		{
			for(int j=0;j<=n+m+1;j++)
			{
				tempobj[i][j]=0;
			}
		}
		
		for(int j=0; j<=n+m+1;j++)
		{
			tempobj[pr][j]= obj[pr][j]/pivot_number;
		}
		
		
		for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
				if(i!=pr)	
				{
					
						multiple = obj[i][pc]/obj[pr][pc];	
		//				printf("\n i=%d j=%d --m=%f --\n",i,j,multiple);
						tempobj[i][j] = obj[i][j]- obj[pr][j] * multiple;
					
				}		
				//obj[i][j]= obj[i][j]-obj[pivot_row][j]*( obj[i][j]/obj[pivot_row][j]);
		//	printf("%f ",obj[i][j]);
				
			}
		//	printf("\n");
		}
	
	for(int i=0;i<=m+1;i++)
		{
		
			for(int j=0;j<=n+m+1;j++)
			{
		
				obj[i][j]=tempobj[i][j];
		
		
			}	
		}
		/*
		for(int i=0;i<(m+1);i++)
		{	
			for(int j=0;j<(n+m+1);j++)
			{
				printf(" %f ",obj[i][j]);
			}
			printf("\n");
    	}
		*/
		//printf("n=%d m=%d \n",n,m);
		for(int i=1;i<(m+1);i++)
		{	
			//if(i!=0)
			printf(" X%d = ",basis[i]);
				
			for(int j=0;j<=(n+m+1);j++)
			{
				if(j==0)
				{
					if(i==0)
					{	
					}
					else
					printf(" %f ",obj[i][j]);
				}
				else if(i==0 && j!=0)
				{
				
				}	
				
				else if(j==basis[i])
				{
					
				}
				else if(i!=0)
			//	printf(" %fX%d ",obj[i][j],j);
			//	else 
				{
						if(obj[i][j]>0)
						printf(" - %fX%d ",obj[i][j],j);	
						else if(obj[i][j]<0)
						{
							float tem= -(obj[i][j]);
							printf(" + %fX%d ",tem,j);	
						}
						else if(obj[i][j]==0)
						printf(" + %fX%d ",obj[i][j],j);
				}
				
				
			}
			printf("\n");
    	}
    
    		float tem= -(obj[0][0]);
			printf("-----------------------------------------------------------------------------\n");
						printf("z= %f ",tem);
    		for(int j=1;j<=n+m+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
    		
			printf("\n");
		
	}
	printf("\n Calculate Z according to original given problem \n");
	for(j=0;j<=n+m+1;j++)
	{
		obj[0][j]=obj[m+1][j];
	}
	for(int i=0;i<(m+1);i++)
		{	
			if(i!=0)
			printf(" X%d = ",basis[i]);
				
			for(int j=0;j<(n+m+1);j++)
			{
				if(j==0)
				{
					if(i==0)
					{	//float tem= -(obj[i][j]);
						//printf("z= %f ",tem);
					}
					else
					printf(" %f ",obj[i][j]);
				}
				else if(i==0 && j!=0)
				{
					
				}	
				
				else if(j==basis[i])
				{
					
				}
				else if(i!=0 )
			//	printf(" %fX%d ",obj[i][j],j);
			//	else 
				{
						if(obj[i][j]>0)
						printf(" - %fX%d ",obj[i][j],j);	
						else if(obj[i][j]<0)
						{
							float tem= -(obj[i][j]);
							printf(" + %fX%d ",tem,j);	
						}
						else if(obj[i][j]==0)
						printf(" + %fX%d ",obj[i][j],j);
				}
				
				
			}
			printf("\n");
    	}
			float tem= -(obj[0][0]);
			printf("-----------------------------------------------------------------------------\n");
			printf("z= %f ",tem);
    		for(int j=1;j<n+m+1;j++)
    		{
    				if(obj[0][j]>0)
					printf(" + %fX%d ",obj[0][j],j);	
					else if(obj[0][j]<0)
					printf(" %fX%d ",obj[0][j],j);	
					else if(obj[0][j]==0)
					printf(" + %fX%d ",obj[0][j],j);	
    		}
    		printf("\n");
	
		
}

