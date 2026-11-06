#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

constexpr int VERTEX_COUNT = 6;
using Edge = tuple<int, int, int>; // (weight, u, v)
using Ticket = tuple<int, int, int>; // (weight, v, from)

char nameOf(int vertex) {
    return static_cast<char>('A' + vertex);
}

string edgeName(int u, int v, int weight) {
    return string(1, nameOf(u)) + "-" + nameOf(v) + "(" + to_string(weight) + ")";
}

class DisjointSet {
public:
    DisjointSet() : parent(VERTEX_COUNT) {
        for (int i = 0; i < VERTEX_COUNT; ++i) {
            parent[i] = i;
        }
    }

    int find(int vertex) {
        if (parent[vertex] != vertex) {
            parent[vertex] = find(parent[vertex]); // 路徑壓縮
        }
        return parent[vertex];
    }

    bool unite(int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        if (rootU == rootV) {
            return false;
        }

        // 編號較小的根當老大，確保 p[] 的根一致。
        if (rootU < rootV) {
            parent[rootV] = rootU;
        } else {
            parent[rootU] = rootV;
        }
        --componentCount;
        return true;
    }

    const vector<int>& parents() const { return parent; }

    int components() const { return componentCount; }

private:
    vector<int> parent;
    int componentCount = VERTEX_COUNT;
};

void printParents(const vector<int>& parent) {
    cout << "    p[A..F] = [";
    for (int i = 0; i < VERTEX_COUNT; ++i) {
        if (i > 0) cout << ", ";
        cout << nameOf(parent[i]);
    }
    cout << "]\n";
}

struct KruskalResult {
    vector<Edge> chosen;
    int totalWeight = 0;
    int components = VERTEX_COUNT;
};

KruskalResult kruskal(const string& title, vector<Edge> edges) {
    sort(edges.begin(), edges.end()); // tuple 依 (w, u, v) 排序

    cout << "\n=== " << title << "：Kruskal ===\n";
    cout << "依 (w, u, v) 排序後逐邊檢查；成環的邊會標示為跳過。\n";

    DisjointSet dsu;
    KruskalResult result;
    int step = 0;

    for (const auto& [weight, u, v] : edges) {
        ++step;
        cout << "  " << step << ". " << edgeName(u, v, weight) << "：";
        if (dsu.unite(u, v)) {
            result.chosen.emplace_back(weight, u, v);
            result.totalWeight += weight;
            cout << "收下\n";
        } else {
            cout << "跳過（成環，兩端已連通）\n";
        }
        printParents(dsu.parents());
    }

    result.components = dsu.components();
    cout << "  收邊順序：";
    for (size_t i = 0; i < result.chosen.size(); ++i) {
        if (i > 0) cout << " → ";
        const auto& [weight, u, v] = result.chosen[i];
        cout << edgeName(u, v, weight);
    }
    if (result.chosen.empty()) cout << "（沒有邊）";
    cout << "\n  總重：" << result.totalWeight << "\n";
    cout << "  連通分量數：" << result.components << "\n";
    if (result.components > 1) {
        cout << "  結果：最小生成森林，包含 " << result.components
             << " 棵樹；原圖不連通，因此不存在涵蓋所有頂點的生成樹。\n";
    }
    return result;
}

struct PrimResult {
    // Prim tracks tree membership directly; it has no disjoint-set parent state.
    vector<Edge> chosen;
    int totalWeight = 0;
};

PrimResult prim(const string& title, const vector<Edge>& edges, int start = 0) {
    vector<vector<pair<int, int>>> graph(VERTEX_COUNT);
    for (const auto& [weight, u, v] : edges) {
        graph[u].push_back({v, weight});
        graph[v].push_back({u, weight});
    }

    priority_queue<Ticket, vector<Ticket>, greater<Ticket>> tickets;
    vector<bool> inTree(VERTEX_COUNT, false);
    PrimResult result;
    tickets.emplace(0, start, -1); // 起點的虛擬票

    cout << "\n=== " << title << "：Prim（從 " << nameOf(start) << " 開始）===\n";
    cout << "priority_queue 存 (w, v, from)，greater 讓最小票先出來。\n";

    while (!tickets.empty()) {
        const auto [weight, v, from] = tickets.top();
        tickets.pop();

        if (inTree[v]) {
            cout << "  丟棄舊票 " << edgeName(from, v, weight)
                 << "（" << nameOf(v) << " 已在樹裡）\n";
            continue;
        }

        inTree[v] = true;
        if (from != -1) {
            result.chosen.emplace_back(weight, from, v);
            result.totalWeight += weight;
            cout << "  收下 " << edgeName(from, v, weight) << "\n";
        } else {
            cout << "  將起點 " << nameOf(v) << " 放入樹\n";
        }

        // 連到已在樹中的點也放入佇列，之後取出時由 inTree 判斷並丟棄舊票。
        for (const auto& [next, nextWeight] : graph[v]) {
            tickets.emplace(nextWeight, next, v);
        }
    }

    cout << "  收邊順序：";
    for (size_t i = 0; i < result.chosen.size(); ++i) {
        if (i > 0) cout << " → ";
        const auto& [weight, u, v] = result.chosen[i];
        cout << edgeName(u, v, weight);
    }
    if (result.chosen.empty()) cout << "（沒有邊）";
    cout << "\n  總重：" << result.totalWeight << "\n";
    return result;
}

void printGraph() {
    cout << "頂點位置輪廓（權重以邊清單為準）：\n"
         << "        B --------5-------- D\n"
         << "       /|\\             / | \\\n"
         << "    4 / |1 \\         8 /  |2 \\ 6\n"
         << "     A  |   \\       /   |   \\ F\n"
         << "      \\2|    C----10----E----3---/\n"
         << "\n邊清單（w, u, v）：\n";
}

int main() {
#ifdef _WIN32
    // Windows 主控台預設字碼頁可能不是 UTF-8；設定後可正確顯示中文與箭頭。
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 頂點 A~F 對應 0~5。每條無向邊以 (weight, u, v) 儲存。
    const vector<Edge> connectedGraph = {
        {4, 0, 1}, // A-B
        {2, 0, 2}, // A-C
        {1, 1, 2}, // B-C
        {5, 1, 3}, // B-D
        {8, 2, 3}, // C-D
        {10, 2, 4}, // C-E
        {2, 3, 4}, // D-E
        {6, 3, 5}, // D-F
        {3, 4, 5}, // E-F
    };

    cout << "頂點編號：A=0, B=1, C=2, D=3, E=4, F=5\n";
    printGraph();
    for (const auto& [weight, u, v] : connectedGraph) {
        cout << "  (" << weight << ", " << nameOf(u) << ", " << nameOf(v) << ")\n";
    }

    const KruskalResult mst = kruskal("連通範例", connectedGraph);
    const PrimResult primMst = prim("連通範例", connectedGraph);
    cout << "\n檢查：Kruskal 與 Prim 的總重"
         << (mst.totalWeight == primMst.totalWeight && mst.totalWeight == 13
                 ? "都是 13。\n"
                 : "不符合預期。\n");

    // 只留下兩個互不相連的三角形／路徑元件，示範 Kruskal 的最小生成森林。
    const vector<Edge> disconnectedGraph = {
        {4, 0, 1}, // A-B
        {2, 0, 2}, // A-C
        {1, 1, 2}, // B-C
        {2, 3, 4}, // D-E
        {3, 4, 5}, // E-F
    };

    cout << "\n=== 不連通圖測試 ===\n"
         << "元件為 {A, B, C} 與 {D, E, F}，兩邊沒有連線。\n";
    const KruskalResult forest = kruskal("不連通範例", disconnectedGraph);
    cout << "  說明：收下 " << forest.chosen.size() << " 條邊、總重 "
         << forest.totalWeight << "，得到 " << forest.components
         << " 個連通分量的最小生成森林。\n";

    return 0;
}
