%%Creation of the file containing the model.

MAT_DIM = 10;
MAX_VALUE = 100;
mat1 = randi(MAX_VALUE, MAT_DIM, MAT_DIM);
mat2 = randi(MAX_VALUE, MAT_DIM, MAT_DIM);

res_expected = mat1*mat2;

cd ..
current_folder = pwd;
ID = fopen (strcat(current_folder,'/irq_test_lib.h'), 'w');
cd matlab_code/

fprintf(ID, '#define MAT_DIM    %d\n\n', MAT_DIM);

name_str = 'uint32_t res[MAT_DIM][MAT_DIM];\n';
fprintf(ID, name_str);

name_str = 'uint32_t mat1[MAT_DIM][MAT_DIM] = {';
print_matrix(ID, mat1, name_str, MAT_DIM);    

name_str = 'uint32_t mat2[MAT_DIM][MAT_DIM] = {';
print_matrix(ID, mat2, name_str, MAT_DIM);

name_str = 'uint32_t res_expected[MAT_DIM][MAT_DIM] = {';
print_matrix(ID, res_expected, name_str, MAT_DIM);

