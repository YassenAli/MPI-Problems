#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, numprocs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if(rank==0){
        printf("Hello from master process.\n");
        fflush(stdout);
	printf("Number of slave processes is %d\n", numprocs - 1);
        int n;
        printf("Enter the size of the array: ");
        fflush(stdout);
	scanf("%d", &n);
        int *array = (int *)malloc(n * sizeof(int));
        printf("\nEnter the elements of the array:\n");
        fflush(stdout);
	for(int i = 0; i < n; i++){
            scanf("%d", &array[i]);
        }
        int elements_per_process = n / numprocs;
        int index, i;

        if(numprocs > 1){
            for(i = 1; i < numprocs - 1; i++){
                index = i * elements_per_process;
                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&array[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            index = i * elements_per_process;
            int elementsLeft = n - index;
            MPI_Send(&elementsLeft, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array[index], elementsLeft, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        int mxIdx = 0;
        int mx = array[0];
        for(int i = 1; i < elements_per_process; i++){
            if(array[i] > mx){
                mx = array[i];
                mxIdx = i;
            }
        }

        int mxIdxGlobal = mxIdx;
        int mxGlobal = mx;
        for(int i = 1; i < numprocs; i++){
            int mxIdxLocal;
            int mxLocal;
            MPI_Recv(&mxIdxLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&mxLocal, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(mxLocal > mxGlobal){
                mxGlobal = mxLocal;
                mxIdxGlobal = mxIdxLocal + (i * elements_per_process);
            }
        }
        printf("Master process announce The maximum element is %d at index %d\n", mxGlobal, mxIdxGlobal);
        fflush(stdout);
	free(array);
    } else {
        int elements_per_process;
        MPI_Recv(&elements_per_process, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int *array = (int *)malloc(elements_per_process * sizeof(int));
        MPI_Recv(array, elements_per_process, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int mxIdx = 0;
        int mx = array[0];
        for(int i = 1; i < elements_per_process; i++){
            if(array[i] > mx){
                mx = array[i];
                mxIdx = i;
            }
        }
        printf("Hello from slave#%d Max number in my partision is %d and index is %d\n", rank, mx, mxIdx);
        fflush(stdout);
	MPI_Send(&mxIdx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&mx, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(array);
    }
    MPI_Finalize();
    if(rank == 0){
        printf("Thanks for using our program.\n");
	fflush(stdout);
    }
    return 0;
}
// Compile with: mpicc -o problem1 problem1.c
// Run with: mpirun -np <number_of_processes> ./problem1
