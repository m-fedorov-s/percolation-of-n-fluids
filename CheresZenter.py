import random

class HexLatt :
    def __init__(self, n):
        self.size=n
        self.good = set()
        self.notvisited = self.good
        self.BoardVisited = False
    
    def InitProbSubs(self):
        self.good = set()
        for i in range(self.size):
            for j in range(self.size+i):
                if random.randrange(2)==1 : self.good |= set([(i, j)])
        for i in range(1, self.size):
            for j in range(i, 2*self.size-1):
                if random.randrange(2)==1 : self.good |= set([(i+self.size-1, j)])
        self.notvisited = self.good
    
    def GetNeighbours(self, x, y):
        return set([(x-1, y-1), (x-1, y), (x, y-1), (x+1, y), (x, y+1), (x+1, y+1)])
    
    def CheckPercol(self):
        self.notvisited = self.good
        self.BoardVisited = False
        self.dfs(self.size-1,self.size-1)
        return self.BoardVisited
    
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
        pr=0
        for i in range(n):
            self.InitProbSubs()
            if self.CheckPercol():
                pr+=1
        return float(pr)/n


def main():
    for i in range (109):
        a = HexLatt(i+2)
        print ("Сторона {0} -> {1}".format(i+2, a.MakeGuesses(2000)))
        del a
    a = input("Rjytw.")
    print(a)
    
    
if __name__=='__main__':
    main()
