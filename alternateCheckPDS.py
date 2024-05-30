#Use python to write GAP code to check the PDSs
#Read("run_output/gap_check_code.g");
import sys

#decompose string x into parameters
def getParms(x):
  openParPDS = 3
  closeParPDS = x.index(") ")
  openParGrp = x.index("SmGrp(") + 5
  closeParGrp = x.index("):")
  firstnums = list(map(int,x[openParPDS+1:closeParPDS].split(",")))
  secondnums = list(map(int,x[openParGrp+1:closeParGrp].split(",")))
  for i in secondnums:
    firstnums.append(i)
  return firstnums

#given a (very specifically formatted) string (line), extract the pds
def get_pds(line):
  colonInd = line.index(":")
  periodInd = line.index(".")
  return [int(b) for b in line[colonInd+1:periodInd].strip().split(" ")]


#CHANGEME put file name in right here
fname = "run_output/PDSs_sorted_no64.txt"
print("#Examine: ", fname)
f = open(fname,"r") #contains list of PDSs to check
data = []
for a in f.readlines(): #read in data
  data.append(a)

f.close()
print("Read(Concatenation(ppath,\"Dr. Smith dset code/11IncidenceMatrices.txt\"));")


for a in data:
  #don't check if PDS not present
  if "None in" in a or ("PDS" not in a): 
    continue
  parms = getParms(a) #extract parameters of PDS
  n = parms[0]
  k = parms[1]
  lam = parms[2]
  mu = parms[3]
  if (n != parms[4]):
    print("bad1, v != n")
    print(n)
    print(parms[4])
    sys.exit(3)
  gid = parms[5]

  pds = get_pds(a) #extract PDS

  
  #the GAP code needed
  print(f"g := SmallGroup({n},{gid}); k := {k}; pds := {[a+1 for a in pds]}; e:=Elements(g); Print(Size(Set(pds))=k and (Set(List(pds,x->Position(e,e[x]^-1)))=Set(pds)) and Degree(MinimalPolynomial(IncidenceMatrix(LeftTranslates_Nums(g,pds))))=3 and (not (1 in pds)),\"\\n\");")
  

 


 

