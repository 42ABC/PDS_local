#Generate a Convolution table for a single group 
#Read("gen_conv_table.g"); //run with this command

#Acknowledgements: Dr. Smith &  Ryan Kaliszewski wrote the IncidenceMatrices, ConvolutionTable code 
Read("11IncidenceMatrices.txt");
Read("1ConvolutionTable.txt");

###CHANGE ME (for the specific group you want)
#n=,id=,g=,fname=
n := 512;
D8 := DihedralGroup(8);
C2 := CyclicGroup(2);
id := 8222222;
g := DirectProduct(D8,C2,C2,C2,C2,C2,C2); #generate a group via direct products, instead of from the SmallGrp library

fname := "table512_D8xC2p6.txt";
###END CHANGE ME

e := Elements(g);
MT := ConvolutionTable_f(g,e,x->x);

#Print out the info to text files to use in Java
PrintTo(fname,n," ", id, "\n");

#print the convolution table to the file
for i in [1..n] do
    for j in [1..n] do
        AppendTo(fname, MT[i][j], " "); 
    od;
    AppendTo(fname, "\n"); 
od;

