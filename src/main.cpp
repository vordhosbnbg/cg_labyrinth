#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <list>
#include <memory>

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
    Position() : row(0), col(0) {}
    Position(int r, int c) : row(r), col(c){}
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

inline size_t getOffset(const Position& pos)
{
    return pos.row*C+pos.col;
}

inline char& getVal(const Position& pos)
{
    return level[getOffset(pos)];
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

inline bool moveRC(const Position& pos, Direction dir, Position& outPos )
{
    bool retVal = false;
    outPos = pos;
    switch (dir)
    {
    case Direction::LEFT:
        if(outPos.col > 0)
        {
            outPos.col--;
            retVal = true;
        }
        break;
    case Direction::RIGHT:
        if(outPos.col < C)
        {
            outPos.col++;
            retVal = true;
        }
        break;
    case Direction::UP:
        if(outPos.row > 0)
        {
            outPos.row--;
            retVal = true;
        }
        break;
    case Direction::DOWN:
        if(outPos.row < R)
        {
            outPos.row++;
            retVal = true;
        }
        break;
    default:
        break;
    }

    return retVal;
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
struct Node;

struct Node
{
    explicit Node():
        pos(0,0),
        parent(nullptr),
        child(nullptr),
        sibling(nullptr),
        visited(false)
    {

    }
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    ~Node() = default;

    Node* addChild(const Position& pos)
    {
        if(!child)
        {
            child.reset(new Node());
            child->pos =  pos;
            child->setParent(this);
            return child.get();
        }
        else
        {
            return child->addSibling(pos);
        }
    }
    Node* addSibling(const Position& pos)
    {
        if(!sibling)
        {
            sibling.reset(new Node());
            sibling->pos = pos;
            sibling->setParent(parent);
            return sibling.get();
        }
        else
        {
            return sibling->addSibling(pos);
        }
    }
    void setParent(Node* node)
    {
        parent = node;
    }
    Position pos;
    Node * parent;
    std::unique_ptr<Node> child;
    std::unique_ptr<Node> sibling;
    PositionPath pathSoFar;
    bool visited;
};

struct Graph
{
    Graph(const Position& pos)
    {
        rootNode.reset(new Node());
        rootNode->pos = pos;
        leafNodes.emplace_back(rootNode.get());
        allNodesMap.resize(R*C, nullptr);
        allNodesMap[getOffset(pos)] = rootNode.get();
    }

    Node* nodeExistsAtPos(const Position& pos)
    {
        return allNodesMap[getOffset(pos)];
    }

    inline void tryGrowInDirection(Node* currentNode, Direction dir, std::list<Node*>& newLeafs)
    {
        Position tempPos;
        if(moveRC(currentNode->pos, dir, tempPos))
        {
            if(!nodeExistsAtPos(tempPos))
            {
                char val = getVal(tempPos);
                switch(val)
                {
                    case '?':
                    case '.':
                    case 'T':
                        Node * childNode = currentNode->addChild(tempPos);
                        childNode->pathSoFar = currentNode->pathSoFar;
                        allNodesMap[getOffset(tempPos)] = childNode;
                        newLeafs.emplace_back(childNode);
                    break;

                }
            }
        }
    }
    inline void growNetwork(bool lookingForUnexplored, bool lookingForT, bool lookingforC)
    {
        std::list<Node*> newLeafs;

        bool saveRemainingLeafs = false;

        for(std::list<Node*>::iterator currentNodeIt = leafNodes.begin();
            currentNodeIt != leafNodes.end();
            ++currentNodeIt)
        {
            Node* currentNode = *currentNodeIt;
            if(saveRemainingLeafs)
            {
                newLeafs.emplace_back(currentNode);
            }
            else
            {
                currentNode->visited = true;
                currentNode->pathSoFar.emplace_back(currentNode->pos);
                char val = getVal(currentNode->pos);

                if(lookingforC && val == 'C')
                {
                    bestPath = currentNode->pathSoFar;
                    saveRemainingLeafs = true;
                }
                else if(lookingForT && val == 'T')
                {
                    bestPath = currentNode->pathSoFar;
                    saveRemainingLeafs = true;
                }
                else if(lookingForUnexplored && val == '?')
                {
                    bestPath = currentNode->pathSoFar;
                    saveRemainingLeafs = true;
                }
                else
                {
                    tryGrowInDirection(currentNode, Direction::UP, newLeafs);
                    tryGrowInDirection(currentNode, Direction::LEFT, newLeafs);
                    tryGrowInDirection(currentNode, Direction::RIGHT, newLeafs);
                    tryGrowInDirection(currentNode, Direction::DOWN, newLeafs);
                }
            }
        }
        leafNodes = newLeafs;
        leafNodeSize = leafNodes.size();
    }

    std::list<Node*> leafNodes;
    size_t leafNodeSize;
    std::unique_ptr<Node> rootNode;
    PositionPath bestPath;
    std::vector<Node*> allNodesMap;
    bool knowC;
    Position posC;
};



inline MovePath getPathBFS(const Position& fromC, const Position& toT)
{
    Graph fromGraph(fromC);
    Graph toGraph(toT);
    while(1)
    {
        for(std::list<Node*>::iterator currentNodeIt = fromGraph.leafNodes.begin();
            currentNodeIt != fromGraph.leafNodes.end();
            ++currentNodeIt)
        {
            Node * fromNode = *currentNodeIt;
            Node * toNodeConnection = toGraph.nodeExistsAtPos(fromNode->pos);

            if(toNodeConnection)
            {
                PositionPath toPath = toNodeConnection->pathSoFar;
                PositionPath fromPath = fromNode->pathSoFar;
                toPath.emplace_back(toNodeConnection->pos);
                std::reverse(toPath.begin(), toPath.end());
                fromPath.insert(fromPath.end(), toPath.begin(), toPath.end());
                return getMovePath(fromPath);
            }
        }

        fromGraph.growNetwork(false, false, false);
        toGraph.growNetwork(false, false, false);
    }
}

inline MovePath getClosestExplorePath(const Position& from)
{
    Graph fromGraph(from);
    while(fromGraph.bestPath.size() == 0)
    {
        fromGraph.growNetwork(true, false, true);
    }

    return getMovePath(fromGraph.bestPath);
}


void readLevelData()
{
    std::vector<char>::iterator cursor = level.begin();
    for (int i = 0; i < R; i++) // read level info
    {
        std::string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
        std::cin >> ROW; std::cin.ignore();
        std::cerr << ROW << std::endl;
        std::copy(ROW.begin(), ROW.end(), cursor);
        cursor += ROW.size();
    }

}

int main()
{
    std::cin >> R >> C >> A; std::cin.ignore();
    std::cerr << R << " "<< C << " " << A << std::endl;

    level = std::vector<char>(R*C);
    bool knowC = false;
    bool knowT = false;
    Position posC;
    Position posT;
    MovePath currentPath;
    size_t currentPathPos = 0;

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

        readLevelData();
        knowC = findSymbol('C', posC);

        if(!knowC)
        {
            currentPath = getClosestExplorePath(kirkPos);
            currentPathPos = 0;
        }
        else if(posC != kirkPos)
        {
            currentPath = getPathBFS(kirkPos, posC);
            currentPathPos = 0;
        }
        else if(posC == kirkPos)
        {
            currentPath = getPathBFS(posC, posT);
            currentPathPos = 0;
        }

        if(currentPathPos < currentPath.size())
        {
            moveOnPath(currentPath, currentPathPos);
        }
        //exit(0);
    }
}
