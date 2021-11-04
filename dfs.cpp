
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <assert.h>

#define WIDTH 10
#define HEIGHT 10

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string blue("\033[0;34m");
const std::string reset("\033[0m");

const std::string redBack("\x1B[41m");
const std::string greenBack("\x1B[42m");
const std::string yellowBack("\x1B[43m");
const std::string blueBack("\x1B[44m");

enum class Colour : uint8_t
{
    Red,
    Blue,
    Yellow,
    Green,
    BackgroundRed = 0x80,
    BackgroundBlue,
    BackgroundYellow,
    BackgroundGreen
};

Colour matrix[WIDTH][HEIGHT];

std::unordered_map<std::string, bool> visited;

std::string getLookup(int x, int y)
{
    return std::to_string(x) + ":" + std::to_string(y);
}

void visit(int x, int y)
{
    visited[getLookup(x,y)] = true;
}

bool isVisited(int x, int y)
{
    auto it = visited.find(getLookup(x, y));
    if (it == std::end(visited))
        return false;

    return it->second;
}

std::vector<std::pair<int, int>> getAdjacent(int x, int y)
{
    std::vector<std::pair<int, int>> nodes;

    if (y > 0)
    {
        nodes.emplace_back(std::make_pair(x , y - 1));
    }
    if (x + 1 <= (WIDTH - 1))
    {
        nodes.emplace_back(std::make_pair(x + 1 , y));
    }
    if (y + 1 <= (HEIGHT - 1))
    {
        nodes.emplace_back(std::make_pair(x , y + 1));
    }
    if (x > 0)
    {
        nodes.emplace_back(std::make_pair(x - 1 , y));
    }

    return nodes;
}

Colour getColour(int x, int y)
{
    assert(x < WIDTH && y < HEIGHT);
    return matrix[x][y];
}

void recursiveDFS(int x , int y, Colour searchColour, std::vector<std::pair<int,int>>& path)
{
    if (isVisited(x, y))
    {
        return;
    }

    if (getColour(x, y) != searchColour)
    {
        return;
    }

    visit(x, y);

    path.emplace_back(std::make_pair(x, y));

    auto adjacent = getAdjacent(x, y);
    for (const auto& n : adjacent)
    {
        recursiveDFS(n.first, n.second, searchColour, path);
    }
}

std::vector<std::pair<int,int>> iterativeDFS(int x, int y)
{
    std::vector<std::pair<int,int>> path;
    std::stack<std::pair<int,int>> m_stack;

    m_stack.emplace(std::make_pair(x,y));

    Colour searchColour = getColour(x, y);
    while (m_stack.empty() == false)
    {
        auto top = m_stack.top();
        m_stack.pop();

        if (isVisited(top.first, top.second))
            continue;

        if (getColour(top.first, top.second) != searchColour)
            continue;

        visit(top.first, top.second);

        path.emplace_back(std::make_pair(top.first, top.second));

        const auto adjacent = getAdjacent(top.first , top.second);
        for (const auto it : adjacent)
        {
            if (isVisited(it.first, it.second))
                continue;

            m_stack.push(it);
        }
    }

    return path;
}

void printPath(const std::vector<std::pair<int, int>>& path)
{
    std::cout << "Longest path:\n";
    for (const auto& it : path)
    {
        std::cout << it.first << ":" << it.second << "\n";
    }
}

void printColour(int x, int y)
{
    Colour c = matrix[x][y];
    switch (c)
    {
    case Colour::Blue:
        std::cout << blue << " B " << reset;
        break;
    case Colour::Red:
        std::cout << red << " R " << reset;
        break;
    case Colour::Yellow:
        std::cout << yellow << " Y " << reset;
        break;
    case Colour::Green:
        std::cout << green << " G " << reset;
        break;
    case Colour::BackgroundBlue:
        std::cout << blueBack << " B " << reset;
        break;
    case Colour::BackgroundRed:
        std::cout << redBack << " R " << reset;
        break;
    case Colour::BackgroundYellow:
        std::cout << yellowBack << " Y " << reset;
        break;
    case Colour::BackgroundGreen:
        std::cout << greenBack << " G " << reset;
        break;
    }
}

void printMatrix(Colour matrix[WIDTH][HEIGHT])
{
    for (int y = 0; y < HEIGHT; ++y)
    {
        std::cout << " " << std::string((4 * WIDTH) + 1, '-') << "\n";
        
        std::cout << " ";
        for (int x = 0; x < WIDTH; ++x)
        {
             std::cout << "|";
             printColour(x,y); 
        }
        std::cout << "|\n";
    }
    std::cout << " " << std::string((4 * WIDTH) + 1, '-') << "\n";
}

int main()
{
    // Generate random data...
    srand(time(nullptr));
    for (int i = 0 ; i < WIDTH; ++i)
        for (int j = 0; j < HEIGHT; ++j)
            matrix[i][j] = static_cast<Colour>(rand() % 4);

    // evaluate each tile
    size_t maxLength = 0;
    std::vector<std::vector<std::pair<int, int>>> longestPaths;
    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            #if RECURSIVE
            std::vector<std::pair<int, int>> path;
            recursiveDFS(x, y, matrix[x][y], path);
            #else
            std::vector<std::pair<int, int>> path = iterativeDFS(x, y);
            #endif

            if (path.size() > maxLength)
            {
                longestPaths.clear();
                maxLength = path.size();
                longestPaths.emplace_back(std::move(path));
            }
            else if (path.size() == maxLength)
            {
                longestPaths.emplace_back(std::move(path));
            }
        }
    }

    // Update the matrix with results
    for (const auto& path : longestPaths)
    {
        printPath(path);
        for (const auto& it : path)
        {
            int c = static_cast<int>(matrix[it.first][it.second]) | 0x80;
            matrix[it.first][it.second] = static_cast<Colour>(c);
        }
    }

    printMatrix(matrix);

    return 0;
}
