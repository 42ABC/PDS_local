#Generate all convolution tables for all groups with order in a specified range
#Note: I recommend skipping order 128 unless if you need those groups, there are a lot of groups of order 128

Read("11IncidenceMatrices.txt");
Read("1ConvolutionTable.txt");

for n in [129..144] do #range of orders for which we are generating tables
  for id in [1..Size(AllSmallGroups(n))] do #for each group of this order

  g := SmallGroup(n,id); #create the group
  e := Elements(g); #get the element list
  MT := ConvolutionTable_f(g,e,x->x); #generate the convolution table
  fname := Concatenation("tablesAll/table",String(n),"_",String(id),".txt"); #file to which we print output
  PrintTo(fname,n," ",id,"\n"); 
  for i in [1..n] do
      for j in [1..n] do
          AppendTo(fname, MT[i][j], " "); 
      od;
      AppendTo(fname, "\n"); 
  od;


  od;
od;