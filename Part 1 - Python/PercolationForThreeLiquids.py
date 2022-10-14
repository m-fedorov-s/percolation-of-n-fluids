# Copyright Fedorov Mikhail, 2019
# fedorov.mikhail.s@gmail.com

import random

class TripleHexLatt :
    def __init__(self, n):
        self.size=n
        self.goodA = set()
        self.goodB = set()
        self.goodC = set()
        self.notvisited = set()
        self.BoardVisited = False
    
    def InitProbSubs(self):
        self.goodA = set()
        self.goodB = set()
        self.goodC = set()
        for i in range(self.size):
            for j in range(self.size+i):
                a = random.randrange(4)
                if a==1 :
                    self.goodA |= set([(i, j)])
                elif a==2:
                    self.goodB |= set([(i, j)])
                elif a==3:
                    self.goodC |= set([(i, j)])
                else:
                    self.goodA |= set([(i, j)])
                    self.goodB |= set([(i, j)])
                    self.goodC |= set([(i, j)])
        for i in range(self.size, 2*self.size-1):
            for j in range(i-self.size+1, 2*self.size-1):
                a = random.randrange(4)
                if a==1 :
                    self.goodA |= set([(i, j)])
                elif a==2:
                    self.goodB |= set([(i, j)])
                elif a==3:
                    self.goodC |= set([(i, j)])
                else:
                    self.goodA |= set([(i, j)])
                    self.goodB |= set([(i, j)])
                    self.goodC |= set([(i, j)])

    def InitThisSubs(self, Cod):
        self.goodA = set()
        self.goodB = set()
        self.goodC = set()
        Ar=Cod
        for i in range(self.size):
            for j in range(self.size+i):
                a = Ar%4
                Ar = Ar//4
                if a==1 :
                    self.goodA |= set([(i, j)])
                elif a==2:
                    self.goodB |= set([(i, j)])
                elif a==3:
                    self.goodC |= set([(i, j)])
                else:
                    self.goodA |= set([(i, j)])
                    self.goodB |= set([(i, j)])
                    self.goodC |= set([(i, j)])
        for i in range(self.size, 2*self.size-1):
            for j in range(i-self.size+1, 2*self.size-1):
                a = Ar%4
                Ar = Ar//4
                if a==1 :
                    self.goodA |= set([(i, j)])
                elif a==2:
                    self.goodB |= set([(i, j)])
                elif a==3:
                    self.goodC |= set([(i, j)])
                else:
                    self.goodA |= set([(i, j)])
                    self.goodB |= set([(i, j)])
                    self.goodC |= set([(i, j)])
    
    def GetNeighbours(self, x, y):
        return set([(x-1, y-1), (x-1, y), (x, y-1), (x+1, y), (x, y+1), (x+1, y+1)])
    
    def CheckPercol(self):
        resres=[]
        self.notvisited = self.goodA
        self.BoardVisited = False
        self.dfs(self.size-1,self.size-1)
        resres.append(self.BoardVisited)
#        if (self.size-1,self.size-1) in self.goodA:
 #           self.dfs(self.size,self.size)
  #          resres.append(self.BoardVisited)
   #     else:
    #        resres.append(False)
        self.notvisited = self.goodB
        self.BoardVisited = False
        self.dfs(self.size-1,self.size-1)
        resres.append(self.BoardVisited)
 #       if (self.size-1,self.size-1) in self.goodB:
  #          self.dfs(self.size,self.size)
   #         resres.append(self.BoardVisited)
    #    else:
     #       resres.append(False)
        self.notvisited = self.goodC
        self.BoardVisited = False
        self.dfs(self.size-1,self.size-1)
        resres.append(self.BoardVisited)
 #       if (self.size-1,self.size-1) in self.goodC:
 #           self.dfs(self.size,self.size)
  #          resres.append(self.BoardVisited)
   #     else:
    #        resres.append(False)
        return resres
    
    def IsInBoard(self, x, y):
        return (x==0) or (x==2*self.size-2) or (y==0) or (y== 2*self.size-2) or (x-y==self.size-1) or (y-x==self.size-1)

    def dfs(self, x, y):
        chast = self.notvisited & self.GetNeighbours(x, y)
        self.notvisited -=chast
        for i in chast:
            if self.IsInBoard(i[0], i[1]):
                self.BoardVisited = True
            if not self.BoardVisited :
                self.dfs(i[0], i[1])
    
    def MakeGuesses(self, n):
        pr=[0, 0, 0, 0]
        for i in range(n):
            self.InitProbSubs()
            ch=self.CheckPercol()
            if (ch[0] and ch[1] and ch[2]):
                pr[3]+=1
            if not ch[0] and not ch[1] and not ch[2]:
                pr[0]+=1
            if (ch[0]and ch[1] and not ch[2])or(ch[2]and ch[0] and not ch[1])or(ch[1]and ch[2] and not ch[0]):
                pr[2]+=1
            if (ch[0]and not ch[1] and not ch[2])or(ch[2]and not ch[0] and not ch[1])or(ch[1]and not ch[2] and not ch[0]):
                pr[1]+=1
        return [float(pr[0])/n, float(pr[1])/n, float(pr[2])/n, float(pr[3])/n]
    
    def FullEnum(self):
        pr=[0, 0, 0, 0]
        for i in range(4**((self.size*2-1)**2-2*(self.size-1)**2+(self.size-2)**2)):
            self.InitThisSubs(i)
            ch=self.CheckPercol()
            if (ch[0] and ch[1] and ch[2]):
                pr[3]+=1
            if not ch[0] and not ch[1] and not ch[2]:
                pr[0]+=1
            if (ch[0]and ch[1] and not ch[2])or(ch[2]and ch[0] and not ch[1])or(ch[1]and ch[2] and not ch[0]):
                pr[2]+=1
            if (ch[0]and not ch[1] and not ch[2])or(ch[2]and not ch[0] and not ch[1])or(ch[1]and not ch[2] and not ch[0]):
                pr[1]+=1
        return pr


def main():
    for i in range (73):
        a = TripleHexLatt(i+2)
        d=a.MakeGuesses(2500)
        print ("Сторона {0} -> Нету: {1}; Одно: {2}; Два: {3}; Три: {4}".format(i+2, d[0], d[1], d[2], d[3]))
        del a
    a = input("Rjytw.")
    print(a)
    
    
if __name__=='__main__':
    main()
