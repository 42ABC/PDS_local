#Given output from SearchMany, sort it so it is easier to read

#decompose string into nums
def sortfun(x):
  openParPDS = 3
  closeParPDS = x.index(") ")
  openParGrp = x.index("SmGrp(") + 5
  closeParGrp = x.index("):")
  firstnums = list(map(int,x[openParPDS+1:closeParPDS].split(",")))
  secondnums = list(map(int,x[openParGrp+1:closeParGrp].split(",")))
  for i in secondnums:
    firstnums.append(i)
  return firstnums
  

f = open("run_output/hS_p1_op.txt","r") #open an output file from SearchMany
data = []
for a in f.readlines(): #for each line in the file
  if "PDS" not in a:
    continue
  data.append(a)
#the first 2 lines are header, so we sort data[2:]
data2 = sorted(data,key=lambda x : sortfun(x)) #sort the data

for i in data2: #print back out to console
  #if "None in " not in i:
  print(i,end="")