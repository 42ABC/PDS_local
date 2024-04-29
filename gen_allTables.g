Read("11IncidenceMatrices.txt");
Read("1ConvolutionTable.txt");

#skip 128!!
for n in [129..144] do
  for id in [1..Size(AllSmallGroups(n))] do

  g := SmallGroup(n,id);
  e := Elements(g);
  MT := ConvolutionTable_f(g,e,x->x);
  fname := Concatenation("tablesAll/table",String(n),"_",String(id),".txt");
  PrintTo(fname,n," ",id,"\n");
  #print the convolution table first
  for i in [1..n] do
      for j in [1..n] do
          AppendTo(fname, MT[i][j], " "); #print the convolution table first
      od;
      AppendTo(fname, "\n"); #print the convolution table first
  od;


  od;
od;