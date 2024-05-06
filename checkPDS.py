#Checks whether a list of sets are indeed PDSs

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
#given the group size (n) and id (gid), open, read, and return the multiplication table for that group
def get_ct(n,gid):
  table_fname = "tablesAll/table" + str(n) + "_" + str(gid) + ".txt"
  tf = open(table_fname,"r")
  tdata = tf.readlines()
  table_id = [int(b) for b in tdata[0].split(" ")]
  if len(table_id) != 2 or table_id[0] != n or table_id[1] != gid:
    print("Bad table read, abort")
    sys.exit(17)
  ct = []
  for line in tdata[1:]:
    ct.append([int(b)-1 for b in line.strip().split(" ")])
  return ct
#do a group ring check to determine whether a set is a regular pds
#(n,k,lam,mu) <- PDS parms
#ct <- multiplication table
#pds <- candidate set
def is_pds(n,k,lam,mu,ct,pds):
  if len(set(pds)) != k: #if pds does not have k elements, is not a pds
    return False
  sums = [0 for i in range(n)]
  for i in pds:
    for j in pds:
      sums[ct[i][j]] += 1 #take pairwise sums
  if sums[0] != k: #if identity does not show up k times in sum, not a regular PDS
    return False
  for i in range(1,n): #start range from 1 skip element 0, the identity
    if (i in pds) and (sums[i] != lam): #if pds elements do not show up lambda times each, not a PDS
      return False
    if (i not in pds) and (sums[i] != mu): #if other elements do not show up mu times each, not a PDS
      return False
  return True
  

f = open("PDSs_sorted_no64_5_4.txt","r") #contains list of PDSs to check
data = []
for a in f.readlines(): #read in data
  data.append(a)

f.close()

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
  
  print(pds)
  ct = get_ct(n,gid) #extract group table
  #print(ct)
  checkPD = is_pds(n,k,lam,mu,ct,pds) #check if PDS
  if not checkPD: #error if a single non-PDS found, as this is a double check, all sets inputted should be PDSs
    print("Bad PDS! ")
    sys.exit(10)
  print(is_pds(n,k,lam,mu,ct,pds)) #print result of check

 


 

