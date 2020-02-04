function print_matrix( ID, matrix, name_str, MAT_DIM)
%PRINT_MATRIX print a matrix in nice C style
%   Detailed explanation goes here

fprintf(ID, name_str);
fprintf(ID, '{');
fprintf(ID, '%d, ', matrix(1, 1:end-1));
fprintf(ID, '%d', matrix(1, end));
fprintf(ID, '},\n');

for i = 2 : MAT_DIM-1
    for j = 1:length(name_str)
        fprintf(ID, ' ');
    end
    
    fprintf(ID, '{');
    
    fprintf(ID, '%d, ', matrix(i, 1:end-1));
    fprintf(ID, '%d', matrix(i, end));
    
    fprintf(ID, '},\n');
end
for j = 1:length(name_str)
    fprintf(ID, ' ');
end

% print last row
fprintf(ID, '{');
fprintf(ID, '%d, ', matrix(MAT_DIM, 1:end-1));
fprintf(ID, '%d', matrix(MAT_DIM, end));
fprintf(ID, '}};\n\n');


end

