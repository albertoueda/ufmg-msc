/** ------------------------------------------------
Projeto e Analise de Algoritmos
2nd module - Graphs

TP1 - Orkato
Student: Alberto Hideki Ueda
Student ID: 2014765817
------------------------------------------------- **/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

# define EPSILON 0.00001

# define DISCONNECTED_GRAPH_QUALITY -1.000

using namespace std;

struct cell
{
    // node id
    int v;

    // friendship and distance
    int f;
    int d;

    // adopted weight based on 'f' and 'd'
    double weight;

    // useful for Kruskal
    int parent;

    // Next element in the linked list
    struct cell* next;

};

typedef struct cell *Cell;

typedef vector<Cell> Graph;

Graph initialize_graph(int n, bool randomized)
{
    Graph g(n);
    for (int i = 1; i < n; i++)
    {
        g[i] = NULL;
    }

    return g;
}

Graph initialize_graph(int n)
{
    return initialize_graph(n, false);
}

/**
Always inserts, never updates.
*/
void insert(Graph* g, int i, int j, int f, int d, double weight)
{
    Cell new_cell = (Cell) malloc(sizeof(cell));
    new_cell->parent = i;
    new_cell->v = j;
    new_cell->f = f;
    new_cell->d = d;
    new_cell->weight = weight;

    // Push front the new cell to g[i]
    new_cell->next = (*g)[i];
    (*g)[i] = new_cell;
}

/**
Avoid to manipulate pointers. Its better to deal with values.
*/
void insert(Graph* g, Cell node_info)
{
    int i = node_info->parent;
    int j = node_info->v;
    int f = node_info->f;
    int d = node_info->d;
    double w = node_info->weight;

    insert(g, i, j, f, d, w);
}

/**
Always inserts, never updates.
*/
void insert(Graph* g, int i, int j, int f, double d)
{
    insert(g, i, j, f, d, -1);
}

bool exists_edge(Cell linked_list, int target, double weight)
{
    for (Cell cell = linked_list; cell != NULL; cell = cell->next)
    {
        if (cell->v == target && weight == -1)
        {
            return true;
        }
        else if (cell->v == target && cell->weight == weight)
        {
            // cout << "Found with same weight! " << cell->parent << "->" << target << endl;
            return true;
        }
    }

    return false;
}

bool exists_edge(Graph g, int source, int target)
{
    return exists_edge(g[source], target, -1);
}

bool exists_edge(Graph g, int source, int target, double weight)
{
    return exists_edge(g[source], target, weight);
}

void print_linked_list(Cell linked_list, bool show_all_info)
{
    for (Cell cell = linked_list; cell != NULL; cell = cell->next)
    {
        if (show_all_info)
        {
            cout << cell->v << ":" << cell->f << '/' << cell->d << ':' << cell->weight << ' ';
        }
        else
        {
            cout << cell->v << ":" << cell->weight << ' ';
        }
    }
    cout << endl;
}

void print_graph(Graph g, bool show_all_info)
{
    cout << endl;

    for (int i = 1; i < g.size(); i++)
    {
        cout << i << ": ";
        print_linked_list(g[i], show_all_info);
    }

    cout << endl;
}

void print_graph(Graph g)
{
    print_graph(g, false);
}

void visit(Graph g, int source, vector<int>* visited)
{
    for (int j = 1; j < g.size(); j++)
    {
        if (exists_edge(g, source, j) && (*visited)[j] == 0)
        {
            (*visited)[j] = 1;
            visit(g, j, visited);
        }
    }
}

/**
Try to visit each node of graph from a single source, with a DFS.
*/
vector<int> single_dfs(Graph g)
{
    vector<int> visited(g.size());
    visited[1] = 1;
    visit(g, 1, &visited);

    return visited;
}

bool connected_graph(Graph g)
{
    vector<int> visited_nodes = single_dfs(g);

    for (int i = 1; i < visited_nodes.size(); i++)
    {
        if (visited_nodes[i] == 0)
        {
            // cout << "Node " << i << " not visited. The graph is not connected." << endl;
            return false;
        }
    }

    return true;
}

vector<Cell> merge(vector<Cell> left, vector<Cell> right)
{
    int i = 0, iLeft = 0, iRight = 0;
    int size = left.size() + right.size();
    vector<Cell> sorted(size);

    while (i < size)
    {
        if (iLeft == left.size() || iRight == right.size())
        {
            break;
        }

        if (left[iLeft]->weight > right[iRight]->weight)
        {
            sorted[i++] = left[iLeft++];
        }
        else
        {
            sorted[i++] = right[iRight++];
        }
    }

    // At maximum one of the conditions below will be true
    while (iRight < right.size())
    {
        sorted[i++] = right[iRight++];
    }

    while (iLeft < left.size())
    {
        sorted[i++] = left[iLeft++];
    }

    return sorted;
}

vector<Cell> mergesort(vector<Cell> cells, int p, int q)
{
    if (p < q)
    {
        int r = (p + q) / 2;
        vector<Cell> left = mergesort(cells, p, r);
        vector<Cell> right = mergesort(cells, r + 1, q);

        return merge(left, right);
    }

    // p == q. Just creates a vector of one element
    vector<Cell> single_vector(1);
    single_vector[0] = cells[p];

    return single_vector;
}

Graph kruskal(const Graph g)
{
    int V = g.size();

    // each v -> new group(v)
    vector<int> colors(V);
    for (int i = 1; i < colors.size(); i++)
    {
        colors[i] = i;
    }

    // get all edges
    vector<Cell> all_edges;
    for (int i = 1; i < V; i++)
    {
        for (Cell cell = g[i]; cell != NULL; cell = cell->next)
        {
            // In this work we will consider just the edges from i to j where i < j
            if (i < cell->v)
            {
                all_edges.push_back(cell);
            }
        }
    }

    // sort edges decreasingly
    vector<Cell> sorted_edges = mergesort(all_edges, 0, all_edges.size() - 1);

    Graph mst = initialize_graph(V);
    int k = 0, mst_size = 0;

    while (mst_size < V-2)
    {
        Cell candidate = sorted_edges[k++];

        if (colors[candidate->parent] != colors[candidate->v])
        {
            insert(&mst, candidate);
            mst_size++;

            int old_color = colors[candidate->v];
            int new_color = colors[candidate->parent];

            // make union based on source color
            for (int i = 1; i < colors.size(); i++)
            {
                if (colors[i] == old_color)
                {
                    colors[i] = new_color;
                }
            }
        }
    }

    return mst;
}

/**
Define a new weight for each edge, based on new quality.
*/
void recalculate_weights(Graph* g, double quality_candidate)
{
    for (int i = 1; i < (*g).size(); i++)
    {
        for (Cell cell = (*g)[i]; cell != NULL; cell = cell->next)
        {
            int f = cell->f;
            int d = cell->d;

            double new_weight = f - (d * quality_candidate);

            cell->weight = new_weight;
        }
    }
}

/**
Just for solution check purposes.
*/
double calculate_quality(Graph g)
{
    double sum_friendship = 0.0;
    double sum_distance = 0.0;

    for (int i = 1; i < g.size(); i++)
    {
        for (Cell cell = g[i]; cell != NULL; cell = cell->next)
        {
            sum_friendship += cell->f;
            sum_distance += cell->d;
        }
    }

    if (sum_distance == 0) {
        // cout << "Probably there is some problem in the input file. Sum of distances equals zero." << endl;
        return 0.0;
    }

    return sum_friendship / sum_distance;
}

int main()
{
    cout.precision(3);
    string line;

    // STDIN Reader
    while (cin)
    {
        getline(cin, line);

        if (isdigit(line[0]) == false)
        {
            break;
        }

        // number of nodes
        int n = atoi(line.c_str());

        // 1-based: n + 1
        Graph g = initialize_graph(n + 1);
        Graph candidate_graph;

        double lower_bound = 0.0;
        double upper_bound = 0.0;
        double quality_candidate = 0.0;
        double best_quality = 0.0;

        // graph info reader. Last line read is a blank line
        while (getline(cin, line) && isdigit(line[0]))
        {
            stringstream stream(line);

            int i, j, f, d, w;
            stream >> i >> j >> f >> d;

            // increases first upper bound to binary search
            upper_bound += f;

            insert(&g, i, j, f, d);
            insert(&g, j, i, f, d);
        }

        /*
        cout << "Read graph:" << endl;
        print_graph(g);
        */

        if (!connected_graph(g))
        {
            cout << DISCONNECTED_GRAPH_QUALITY << endl;
            continue;
        }

        // Determine the best quality using a binary search
        while (abs(upper_bound - lower_bound) > EPSILON)
        {
            double sum_weights = 0.0;

            // calculates the next candidate for quality
            quality_candidate = (upper_bound + lower_bound) / 2;

            recalculate_weights(&g, quality_candidate);

            candidate_graph = kruskal(g);

            // add edges of MST and also edges that will increase the ratio
            // if  w > 0  =>  f - r*d > 0  =>  f > r*d   =>  f/d > r
            for (int i = 1; i < candidate_graph.size(); i++)
            {
                for (Cell cell = g[i]; cell != NULL; cell = cell->next)
                {
                    if (exists_edge(candidate_graph, i, cell->v, cell->weight))
                    {
                        sum_weights += cell->weight;
                    }
                    else if (i < cell->v && cell->weight > 0)
                    {
                        sum_weights += cell->weight;

                        // For visualization of solution graph
                        insert(&candidate_graph, cell);
                    }
                }
            }

            // stop if the sum of weights is good enough
            if (abs(sum_weights) <= EPSILON)
            {
                break;
            }

            // if is not, calculates the new lower and topper bounds
            if (sum_weights > 0)
            {
                lower_bound = quality_candidate;
            }
            else
            {
                upper_bound = quality_candidate;
            }
        }


        cout << "last candidate:";
        print_graph(candidate_graph);


        // Output Optimal Quality
        cout << fixed << quality_candidate << endl;
    }
}
