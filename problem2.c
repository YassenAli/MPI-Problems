#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* encrypt(int argc, char* argv[], char* input, int n) {
    char* final_output = (char*)malloc((n + 1) * sizeof(char));

    int rank, number_of_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    int size = n / number_of_processes;
    int remainder = n % number_of_processes;
    int local_size = (rank < remainder) ? size + 1 : size;

    char *local_input = (char*)malloc(local_size * sizeof(char));
    char *local_output = (char*)malloc(local_size * sizeof(char));

    if (rank == 0) {
        int index = 0;
        for (int p = 1; p < number_of_processes; p++) {
            MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);

            index += (p - 1 < remainder) ? size + 1 : size;
            int chunk_size = (p < remainder) ? size + 1 : size;
            MPI_Send(&input[index], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD);
        }
        memcpy(local_input, input, local_size);
    }
    else {
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(local_input, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < local_size; i++) {
        if (local_input[i] >= 'a' && local_input[i] <= 'z')
            local_output[i] = ((local_input[i] - 'a' + 3) % 26) + 'a';
        else if (local_input[i] >= 'A' && local_input[i] <= 'Z')
            local_output[i] = ((local_input[i] - 'A' + 3) % 26) + 'A';
        else
            local_output[i] = local_input[i];
    }

    if (rank == 0) {
        int index = local_size;
        memcpy(final_output, local_output, local_size);
        for (int p = 1; p < number_of_processes; p++) {
            int chunk_size = (p < remainder) ? size + 1 : size;
            MPI_Recv(&final_output[index], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            index += chunk_size;
        }
        final_output[n] = '\0';
    }
    else {
        MPI_Send(local_output, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    free(local_input);
    free(local_output);

    return (rank == 0) ? final_output : NULL;
 }

char* decrypt(int argc, char* argv[], char* input, int n) {
    char* final_output = (char*)malloc((n + 1) * sizeof(char));

    int rank, number_of_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    int size = n / number_of_processes;
    int remainder = n % number_of_processes;
    int local_size = (rank < remainder) ? size + 1 : size;

    char *local_input = (char*)malloc(local_size * sizeof(char));
    char *local_output = (char*)malloc(local_size * sizeof(char));

    if (rank == 0) {
        int index = 0;
        for (int p = 1; p < number_of_processes; p++) {
            MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);

            index += (p - 1 < remainder) ? size + 1 : size;
            int chunk_size = (p < remainder) ? size + 1 : size;
            MPI_Send(&input[index], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD);
        }
        memcpy(local_input, input, local_size);
    }
    else {
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(local_input, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < local_size; i++) {
        if (local_input[i] >= 'a' && local_input[i] <= 'z')
            local_output[i] = ((local_input[i] - 'a' - 3) % 26) + 'a';
        else if (local_input[i] >= 'A' && local_input[i] <= 'Z')
            local_output[i] = ((local_input[i] - 'A' - 3) % 26) + 'A';
        else
            local_output[i] = local_input[i];
    }

    if (rank == 0) {
        int index = local_size;
        memcpy(final_output, local_output, local_size);
        for (int p = 1; p < number_of_processes; p++) {
            int chunk_size = (p < remainder) ? size + 1 : size;
            MPI_Recv(&final_output[index], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            index += chunk_size;
        }
        final_output[n] = '\0';
    }
    else {
        MPI_Send(local_output, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    free(local_input);
    free(local_output);

    return (rank == 0) ? final_output : NULL;
 }

void mainMenu() {
	printf("1. Encrypt\n");
	printf("2. Decrypt\n");
	printf("3. Exit\n");
	printf("Enter your choice: ");
}

int main(int argc, char* argv[]) {
    char input[200];
    char final_output[200];

    MPI_Init(&argc, &argv);

    int rank, number_of_processes;
    int choice = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    while (1) {
        if (choice == 3) break;

        if (rank == 0) {
            mainMenu();
            scanf("%d", &choice);

            for (int p = 1; p < number_of_processes; p++) {
                MPI_Send(&choice, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&choice, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int n;
        if (rank == 0) {
            printf("Enter the string to be %s: ", (choice == 1) ? "encrypted" : "decrypted");
            fflush(stdout);
            scanf("%s", input);
            n = strlen(input);
            for (int p = 1; p < number_of_processes; p++) {
                MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
                MPI_Send(input, n, MPI_CHAR, p, 0, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(input, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        char *result = (choice == 1) ? encrypt(argc, argv, input, n) : decrypt(argc, argv, input, n);

        if (rank == 0) {
            printf("%s\n", result);
            free(result);
            choice = 3;
            for (int p = 1; p < number_of_processes; p++) {
                MPI_Send(&choice, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&choice, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();

    return 0;
}

//
//MPI_Init(&argc, &argv);
//
//    int rank, number_of_processes;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
//
//    int n;
//    if (rank == 0) {
//        printf("Enter the string to be encrypted: ");
//        fflush(stdout);
//        scanf("%s", input);
//        n = strlen(input);
//        printf("Length of string: %d\n", n);
//    }
//
//    // Broadcast the length of the string to all processes
//    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//    int size = n / number_of_processes;
//    int remainder = n % number_of_processes;
//    int local_size = (rank < remainder) ? size + 1 : size;
//
//    // Allocate buffer for local data
//    char* local_input = (char*)malloc(local_size * sizeof(char));
//    char* local_output = (char*)malloc(local_size * sizeof(char));
//
//    // Send data to each process
//    if (rank == 0) {
//        int offset = 0;
//        for (int p = 1; p < number_of_processes; p++) {
//            int chunk_size = (p < remainder) ? size + 1 : size;
//            offset += (p == 1) ? 0 : ((p - 1 < remainder) ? size + 1 : size);
//            MPI_Send(&input[offset], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD);
//        }
//        // Copy own chunk
//        memcpy(local_input, input, local_size);
//    } else {
//        MPI_Recv(local_input, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//    }
//
//    // Encrypt local data
//    for (int i = 0; i < local_size; i++) {
//        if (local_input[i] >= 'a' && local_input[i] <= 'z')
//            local_output[i] = ((local_input[i] - 'a' + 3) % 26) + 'a';
//        else
//            local_output[i] = local_input[i]; // non-lowercase
//    }
//
//    // Gather encrypted chunks back to root
//    if (rank == 0) {
//        int offset = 0;
//        memcpy(final_output, local_output, local_size);
//        offset += local_size;
//        for (int p = 1; p < number_of_processes; p++) {
//            int chunk_size = (p < remainder) ? size + 1 : size;
//            MPI_Recv(&final_output[offset], chunk_size, MPI_CHAR, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//            offset += chunk_size;
//        }
//        final_output[n] = '\0';
//        printf("Encrypted string: %s\n", final_output);
//    } else {
//        MPI_Send(local_output, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
//    }
//
//    free(local_input);
//    free(local_output);
//
//    MPI_Finalize();