#Find PDSs with k=18 in grps of order 64 with an EA(8) image\
#run with 
#Read("gen_conv_table.g");
#Acknowledgements: Dr. Smith wrote the IncidenceMatrices, ConvolutionTable code 
Read("11IncidenceMatrices.txt");
Read("1ConvolutionTable.txt");


#n := 512;
#g := DirectProduct(D8,D8,D8);
n := 144;
C2 := CyclicGroup(2);
C3 := CyclicGroup(3);
D8 := DihedralGroup(8);
id := 2222333;
#g := DirectProduct(C2,C2,C2,C2,C2,C2,C2,C2,C2);
g := DirectProduct(C2,C2,C2,C2,C3,C3,C3);
#g := SmallGroup(n,id); #EA(64)

e := Elements(g);
#Convolution table method
MT := ConvolutionTable_f(g,e,x->x);
#fname := "table_512-d8cubed.txt";
fname := "table_144_2222333.txt";

#Print out the info to text files to use in Java
PrintTo(fname,n," ", id, "\n");


#print the convolution table first
for i in [1..n] do
    for j in [1..n] do
        AppendTo(fname, MT[i][j], " "); #print the convolution table first
    od;
    AppendTo(fname, "\n"); #print the convolution table first
od;

