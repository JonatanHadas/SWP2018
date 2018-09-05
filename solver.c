#include "solver.h"

#include "gurobi_c.h"

#include <stdlib.h> /* malloc */

#include <stdio.h> /* for formating gurobi condition names */

int count_solutions(Board* board){
	int empty_num; /* number of empty places */
	
	/*
	each simulated recursive call gets the next empty position as parameters
	
	these will be saved to x_stack and y_stack (x and y coordinates)
	*/
	int* x_stack;
	int* y_stack;
	int stack_head = 0; /* position of stack head */
	int count = 0;  /* solution counter */
	
	
	if(check_board(board)) {return 0;} /* board is erronous */
	
	x_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	y_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	
	/* positions on the stack always remain the same, so we will calculate them before the recursion */
	empty_num = count_empty_places(board, x_stack, y_stack);
		
	while(stack_head >= 0){ /* as long as stack is not empty */
		if(stack_head == empty_num){
			count++; /* board full */
			stack_head--; /* go back */
		}
		else{
			/* increase current position */
			board->table[y_stack[stack_head]][x_stack[stack_head]] ++;
			
			
			if(board->table[y_stack[stack_head]][x_stack[stack_head]] > board->cell_w * board->cell_h){
				/* all options for position were checked */
				board->table[y_stack[stack_head]][x_stack[stack_head]] = 0; /* reset as empty */
				stack_head--; /* go back */
			}
			else{
				if(!check_position(board, x_stack[stack_head], y_stack[stack_head])){
					/* if no errors were created, go to next position */
					stack_head++;
				}
				/* otherwise next iteration will check next option for this position */
			}
		}
	}
	
	free(x_stack);
	free(y_stack);
	
	return count;
}

Board* solve(Board* board){
	/* gurobi environment and model */
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int cell_w = board->cell_w, cell_h = board->cell_h, N = cell_w*cell_h; /* for convenience and readability */
	double *sol; /* for retreving  solution */
	int *ind; /* for setting confinements */
	double *val;
	double *obj; /* objective function */
	char *vtype; /* for setting to binary type */
	int optimstatus; /* gurobi status */
	int i,x,y,cell_x,cell_y,num; /* for loops */
	Board* new_board; /* for returning solution */

	/* allocations, N^3 variables, N variables per condition */
	sol = calloc(N*N*N, sizeof(double));
	if(sol == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		return NULL;
	}
	vtype = calloc(N*N*N, sizeof(char));
	if(vtype == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(sol);
		return NULL;
	}
	ind = calloc(N, sizeof(int));
	if(ind == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(sol);
		free(vtype);
		return NULL;
	}
	val = calloc(N, sizeof(double));
	if(val == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(sol);
		free(vtype);
		free(ind);
		return NULL;
	}
	obj = calloc(N*N*N, sizeof(double));
	if(val == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(sol);
		free(vtype);
		free(ind);
		free(val);
		return NULL;
	}
	
	/* initialize gurobi, might be errors */
	if(GRBloadenv(&env, "sudoku_gurobi.log")){
		free(sol);
		free(vtype);
		free(ind);
		free(val);
		free(obj);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	if(GRBsetintparam(env, "LogToConsole", 0)){ /* silence gurobi */
		free(sol);
		free(vtype);
		free(ind);
		free(val);
		free(obj);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	if(GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL)){
		free(sol);
		free(vtype);
		free(ind);
		free(val);
		free(obj);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	
	for(i=0; i<N*N*N; i++) {vtype[i] = GRB_BINARY;obj[i] = 0.0;}
	
	if(GRBaddvars(model, N*N*N, 0, NULL, NULL, NULL, NULL, NULL, NULL, vtype, NULL)){
		/* error */
		free(sol);
		free(vtype);
		free(ind);
		free(val);
		free(obj);
		GRBfreemodel(model);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	
	free(obj);
	free(vtype);

	if(GRBupdatemodel(model)){
		/* error */
		free(sol);
		free(ind);
		free(val);
		GRBfreemodel(model);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	/* one number per cell */
	for(x=0;x<N;x++){
		for(y=0;y<N;y++){
			char name[9]; /* name of condition */
			sprintf(name, "cell%2d%2d",x,y);
			if(board->table[y][x] == 0){ /* position is not set */
				for(num = 0; num < N; num++){
					ind[num] = N*N*x + N*y + num;
					val[num] = 1;
				}
				if(GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, name)){
					free(sol);
					free(ind);
					free(val);
					GRBfreemodel(model);
					GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
					return NULL;
				}
			}
			else{
				num = board->table[y][x]-1;
				ind[0] = N*N*x + N*y + num;
				val[num] = 1;
				if(GRBaddconstr(model, 1, ind, val, GRB_EQUAL, 1, name)){
					free(sol);
					free(ind);
					free(val);
					GRBfreemodel(model);
					GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
					return NULL;
				}
			}
		}
	}
		
	/* one appearence per row */
	for(y=0;y<N;y++){
		for(num=0;num<N;num++){
			char name[8];
			sprintf(name, "row%2d%2d",num,y);
			for(x = 0; x < N; x++){
				ind[x] = N*N*x + N*y + num;
				val[x] = 1;				
			}
			if(GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, name)){
				free(sol);
				free(ind);
				free(val);
				GRBfreemodel(model);
				GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
				return NULL;
			}
		}
	}
	
	/* one appearence per column */
	for(x=0;x<N;x++){
		for(num=0;num<N;num++){
			char name[8];
			sprintf(name, "col%2d%2d",num,x);
			for(y = 0; y < N; y++){
				ind[y] = N*N*x + N*y + num;
				val[y] = 1;				
			}
			if(GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, name)){
				free(sol);
				free(ind);
				free(val);
				GRBfreemodel(model);
				GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
				return NULL;
			}
		}
	}
	
	/* one appearence per block */
	for(cell_x=0;cell_x<cell_h;cell_x++){
		for(cell_y=0;cell_y<cell_w;cell_y++){
			int bx,by; /* for loop over block */
			for(num=0;num<N;num++){
				char name[12];
				sprintf(name, "block%2d%2d%2d",num,cell_x,cell_y);
				for(bx = 0; bx<cell_w;bx++){
					for(by = 0; by<cell_h;by++){
						i = bx*cell_h + by;
						x = bx+cell_x*cell_w;
						y = by+cell_y*cell_h;
						ind[i] = N*N*x + N*y + num;
						val[i] = 1;
					}
				}
				if(GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1, name)){
					free(sol);
					free(ind);
					free(val);
					GRBfreemodel(model);
					GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
					return NULL;
				}
			}
		}
	}
		
	free(ind);
	free(val);
	
	if(GRBoptimize(model)){
		free(sol);
		GRBfreemodel(model);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	/* get status */
	if(GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus)){
		free(sol);
		GRBfreemodel(model);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	if(optimstatus == GRB_INF_OR_UNBD || optimstatus == GRB_UNBOUNDED || optimstatus == GRB_INFEASIBLE){
		free(sol);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return board; /* no solution */
	}
	if(optimstatus != GRB_OPTIMAL){
		free(sol);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return NULL; /* some problem */
	}
	
	/* get solution */
	if(GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, N*N*N, sol)){
		free(sol);
		GRBfreemodel(model);
		GRBfreeenv(env);
		fprintf(stderr,"Error in Gurobi: %s\n", GRBgeterrormsg(env));
		return NULL;
	}
	
	GRBfreemodel(model);
	GRBfreeenv(env);
	
	/* if this point is reached there is a solution in sol */
	new_board = copy_board(board);
	
	if(new_board == NULL){
		free(sol);
		return NULL;
	}
	
	for(x=0;x<N;x++)for(y=0;y<N;y++)for(num=0;num<N;num++) if(sol[N*N*x+N*y+num] > 0.5 /* ==1 */)
		new_board->table[y][x] = num+1; /* set the number */
	
	free(sol);
	
	return new_board;
}
