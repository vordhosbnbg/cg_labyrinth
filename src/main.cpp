
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <list>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

enum class Direction
{
    LEFT,
    RIGHT,
    DOWN,
    UP
};

struct Position
{
    inline bool operator ==(const Position& posOther) const
    {
        return ((this->row == posOther.row) && (this->col == posOther.col));
    }

    inline bool operator !=(const Position& posOther)
    {
        return !((*this) == posOther);
    }
    int row;
    int col;
};


using MovePath=std::vector<Direction>;
using PositionPath=std::vector<Position>;
using PositionVec=std::vector<Position>;

static int R; // number of rows.
static int C; // number of columns.
static int A; // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
static Position kirkPos; // where Kirk is located.
static std::vector<char> level;

inline char& getVal(const Position& pos)
{
    return level[pos.row*C+pos.col];
}

inline Position getPos(size_t offset)
{
    Position pos;

    pos.row = offset / C;
    pos.col = offset % C;

    return pos;
}

inline bool findSymbol(char symbol, Position& pos)
{
    bool retVal = false;
    std::vector<char>::iterator it = std::find(level.begin(), level.end(), symbol);
    if(it != level.end())
    {
        int bufPos = it - level.begin();
        pos = getPos(bufPos);
        retVal = true;
    }
    return retVal;
}

inline Position moveRC(const Position& pos, Direction dir)
{
    Position retVal = pos;
    switch (dir)
    {
    case Direction::LEFT:
        retVal.col--;
        break;
    case Direction::RIGHT:
        retVal.col++;
        break;
    case Direction::UP:
        retVal.row--;
        break;
    case Direction::DOWN:
        retVal.row++;
        break;
    default:
        break;
    }

    return retVal;
}

inline bool validToMove(char c)
{
    return (c == '.') || (c == 'T') || (c == 'C');
}

inline PositionVec getNextPossiblePositions(PositionVec& path)
{
    PositionVec result;
    Position nextPos;
    nextPos = moveRC(path.back(), Direction::LEFT);
    if(validToMove(getVal(nextPos))  && !(std::find(path.rbegin(), path.rend(), nextPos) != path.rend()))
    {
        result.emplace_back(nextPos);
    }
    nextPos = moveRC(path.back(), Direction::RIGHT);
    if(validToMove(getVal(nextPos)) && !(std::find(path.rbegin(), path.rend(), nextPos) != path.rend()))
    {
        result.emplace_back(nextPos);
    }
    nextPos = moveRC(path.back(), Direction::UP);
    if(validToMove(getVal(nextPos)) && !(std::find(path.rbegin(), path.rend(), nextPos) != path.rend()))
    {
        result.emplace_back(nextPos);
    }
    nextPos = moveRC(path.back(), Direction::DOWN);
    if(validToMove(getVal(nextPos)) && !(std::find(path.rbegin(), path.rend(), nextPos) != path.rend()))
    {
        result.emplace_back(nextPos);
    }
    return result;
}

inline MovePath getMovePath(PositionPath& posPath)
{
    MovePath movePath;
    for(PositionPath::iterator posPathIt = posPath.begin(); posPathIt+1 != posPath.end(); ++posPathIt)
    {
        Position currentPos = *posPathIt;
        Position nextPos = *(posPathIt+1);
        if(nextPos.col < currentPos.col)
        {
            movePath.emplace_back(Direction::LEFT);
        }
        else if(nextPos.col > currentPos.col)
        {
            movePath.emplace_back(Direction::RIGHT);
        }
        else if(nextPos.row < currentPos.row)
        {
            movePath.emplace_back(Direction::UP);
        }
        else if(nextPos.row > currentPos.row)
        {
            movePath.emplace_back(Direction::DOWN);
        }
    }
    return movePath;
}

inline void getPaths(const Position& from, const Position& to, std::vector<MovePath>& outPaths)
{
    std::list<PositionPath> listOfPathsAsPositions;

    if((getVal(from) != '#') &&
            (getVal(to) != '#'))
    {
        bool exhausted = false;
        bool foundAtLeastOnePath = false;
        PositionPath initialPath;
        initialPath.emplace_back(from);
        listOfPathsAsPositions.emplace_back(initialPath);



        while(!exhausted)
        {
            foundAtLeastOnePath = false;
            for(std::list<PositionPath>::iterator positionPathIt = listOfPathsAsPositions.begin();
                positionPathIt != listOfPathsAsPositions.end();
                )
            {

                PositionPath positionPath = *positionPathIt;
                std::list<PositionPath>::iterator positionPathItCopy = positionPathIt;
                PositionVec possibleMoves;
                if(positionPath.back() != to)
                {
                    possibleMoves = getNextPossiblePositions(positionPath);
                }
                if(possibleMoves.size() > 0)
                {
                    foundAtLeastOnePath = true;
                    for(Position& possibleMove : possibleMoves)
                    {
                        PositionPath newPath = positionPath;
                        newPath.emplace_back(possibleMove);
                        listOfPathsAsPositions.emplace_front(newPath);
                        if(possibleMove == to)
                        {
                            outPaths.emplace_back(getMovePath(newPath));
                        }
                    }
                    ++positionPathIt;
                    listOfPathsAsPositions.erase(positionPathItCopy);
                }
                else
                {
                    ++positionPathIt;
                }
            }


            if(!foundAtLeastOnePath)
            {
                exhausted = true;
            }
        }
    }
    //std::cerr << retVal.size() << " paths found." << std::endl;
}

inline int getExploredPercent(std::vector<char> vec)
{
    int fog = vec.size();
    for(char c : vec)
    {
        if(c != '?')
        {
            fog--;
        }
    }
    return (100*fog)/vec.size();
}

void getListOfPossibleExplorePoints(PositionVec& outPoints)
{
    outPoints.clear();
    for(size_t walk = 0; walk < level.size(); walk++)
    {
        Position currentPos = getPos(walk);
        Position leftPos = moveRC(currentPos, Direction::LEFT);
        Position rightPos = moveRC(currentPos, Direction::RIGHT);
        Position upPos = moveRC(currentPos, Direction::UP);
        Position downPos = moveRC(currentPos, Direction::DOWN);
        if(getVal(currentPos) == '.')
        {
            if(getVal(leftPos) == '?')
            {
                    outPoints.emplace_back(currentPos);
            }
            if(getVal(rightPos) == '?')
            {
                    outPoints.emplace_back(currentPos);
            }
            if(getVal(upPos) == '?')
            {
                    outPoints.emplace_back(currentPos);
            }
            if(getVal(downPos) == '?')
            {
                    outPoints.emplace_back(currentPos);
            }
        }
    }
}

inline MovePath& getShortestPath(std::vector<MovePath>& pathVec)
{
    size_t minSize = UINT32_MAX;
    std::vector<MovePath>::iterator minIt;
    for(std::vector<MovePath>::iterator pathWalk = pathVec.begin(); pathWalk != pathVec.end(); ++pathWalk)
    {
        size_t curPathSize = (*pathWalk).size();
        if(curPathSize < minSize)
        {
            minSize = curPathSize;
            minIt = pathWalk;
        }
    }
    return *minIt;
}

void moveOnPath(MovePath& path, size_t& pathPos)
{
    switch (path[pathPos])
    {
    case Direction::LEFT:
        std::cout << "LEFT" << std::endl;
        break;
    case Direction::RIGHT:
        std::cout << "RIGHT" << std::endl;
        break;
    case Direction::UP:
        std::cout << "UP" << std::endl;
        break;
    case Direction::DOWN:
        std::cout << "DOWN" << std::endl;
        break;
    default:
        break;
    }
    pathPos++;
}

void getBruteForceAllPaths (const PositionVec& explorePoints, std::vector<MovePath>& outPaths)
{
    outPaths.clear();
    std::vector<MovePath> explorePathsForSinglePos;
    for(const Position& pos : explorePoints)
    {
        explorePathsForSinglePos.clear();
        getPaths(kirkPos, pos, outPaths);
    }
}

int main()
{
    std::cin >> R >> C >> A; std::cin.ignore();
    std::cerr << R << " "<< C << " " << A << std::endl;

    level = std::vector<char>(R*C);
    bool reachedC = false;
    bool knowC = false;
    bool knowT = false;
    Position posC;
    Position posT;
    MovePath currentPath;
    size_t currentPathPos;
    std::vector<char>::iterator cursor;
    PositionVec explorePoints;
    std::vector<MovePath> explorePaths;


    // game loop
    while (1)
    {
        std::cin >> kirkPos.row >> kirkPos.col; std::cin.ignore(); // get Kirk location
        std::cerr << kirkPos.row << " " << kirkPos.col << std::endl;
        if(!knowT)
        {
            posT = kirkPos;
            knowT = true;
        }

        cursor = level.begin();
        for (int i = 0; i < R; i++) // read level info
        {
            std::string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
            std::cin >> ROW; std::cin.ignore();
            std::cerr << ROW << std::endl;
            std::copy(ROW.begin(), ROW.end(), cursor);
            cursor += ROW.size();
        }

        if(!knowC)
        {
            knowC = findSymbol('C', posC);
        }

        getListOfPossibleExplorePoints(explorePoints);


        if(currentPathPos < currentPath.size())
        {
            moveOnPath(currentPath, currentPathPos);
        }
        else if(explorePoints.size() > 0)
        {
            explorePaths.clear();
            getBruteForceAllPaths(explorePoints, explorePaths);
            currentPath = getShortestPath(explorePaths);
            currentPathPos = 0;
        }
        else if(!reachedC)
        {
            if(knowC)
            {
                std::vector<MovePath> pathsToC;
                getPaths(kirkPos, posC, pathsToC);
                currentPath = getShortestPath(pathsToC);
                currentPathPos = 0;
            }
        }
        else if (posC == kirkPos)
        {
            reachedC = true;
            std::vector<MovePath> pathsToT;
            getPaths(kirkPos, posT, pathsToT);
            currentPath = getShortestPath(pathsToT);
            currentPathPos = 0;
        }

    }
}
