#Read("gen_tables64.g"); #run with this command
#Acknowledgements: Dr. Smith & Ryan Kaliszewski wrote the IncidenceMatrices, ConvolutionTable code 
Read("11IncidenceMatrices.txt");
Read("1ConvolutionTable.txt");

n := 64;
for id in [1..267] do #range of group ids for which you want to generate tables
    g := SmallGroup(n,id); #create the group
    e := Elements(g); #get list of group elements
    MT := ConvolutionTable_f(g,e,x->x); #create the convolution table
    fname := Concatenation("tables64/table",String(id),".txt");
    PrintTo(fname,n," ",id,"\n");
    for i in [1..n] do #print the convolution table to the file
        for j in [1..n] do
            AppendTo(fname, MT[i][j], " "); 
        od;
        AppendTo(fname, "\n");
    od;


od;