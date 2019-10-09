# Copyright Fedorov Mikhail, 2019
# fedorov.mikhail.s@gmail.com

import random
#Короткая программа описывающая классическую модель протекания.
#Шестиугольная решетка представлена двумерным массивом, каждый
# элемент которого отвечает за протекание жидкости через данную клетку.
class HexLatt :
#Класс HexLatt хранит в себе все данный протекания жидкостей через клетки, 
#структуру шестиугольной решетки и методы для работы с этими данными.
    def __init__(self, n): #конструктор класса
        self.size = n #количество клеток в сотороне шестиугольника
        self.good = set()
        self.notvisited = self.good
        self.BoardVisited = False
    
    def InitProbSubs(self): # Функция создает случайную раскраску клеток шестиугольника.
        self.good = set()
        for i in range(self.size):
            for j in range(self.size+i):
                if random.randrange(2)==1 : self.good |= set([(i, j)])
        for i in range(1, self.size):
            for j in range(i, 2*self.size-1):
                if random.randrange(2)==1 : self.good |= set([(i+self.size-1, j)])
        self.notvisited = self.good
		#Храним все координаты клеток, через которые протекает жидкость в множестве self.good
    
    def GetNeighbours(self, x, y): #функция по координатам клетки выдает набор координат шести соседних клеток
        return set([(x-1, y-1), (x-1, y), (x, y-1), (x+1, y), (x, y+1), (x+1, y+1)])
    
    def CheckPercol(self): #Чтобы проверить, протекает ли жидкость, совершим обход из центральной клетки и проверим, дойдем ли до границы.
        self.notvisited = self.good #Ходим только по клеткам, по которым протекает жидкость.
        self.BoardVisited = False
        self.dfs(self.size-1,self.size-1) #начинаем обход из центральной клетки.
        return self.BoardVisited
    
    def IsInBoard(self, x, y): #функция проверяет, лежит ли клетка с данными координатами на границе шестиугольника
        return (x==0) or (x==2*self.size-2) or (y==0) or (y== 2*self.size-2) or (x-y==self.size-1) or (y-x==self.size-1)

    def dfs(self, x, y): # стандатный алгоритм обхода в глубину. Песещаем клетки рекрусивно, переходя по стороне, отмечая посещенные клетки.
        chast = self.notvisited & self.GetNeighbours(x, y) #все соседи текущей клетки
        self.notvisited -=chast
        for i in chast: #рекурсивно запускаем обход во всех соседних клетках, которые еще не были посещены, и через которые протекает жидкость
            if self.IsInBoard(i[0], i[1]):
                self.BoardVisited = True
            if not self.BoardVisited :
                self.dfs(i[0], i[1])
    
    def MakeGuesses(self, n): #Основной интерфейс для работы с классом. Получает на вход число и проводит данное количество экспериментов. Возвращает долю тех из них, в котором протекла жидкость.
        pr=0
        for i in range(n):
            self.InitProbSubs()
            if self.CheckPercol():
                pr+=1
        return float(pr)/n


def main():
    for i in range (2,110):
        a = HexLatt(i) #инициализируем обект класса для задаанного размера.
        print ("Сторона {0} -> {1}".format(i, a.MakeGuesses(2000)))
        del a
    a = input("Finished.")
    print(a)
    
    
if __name__=='__main__':
    main()