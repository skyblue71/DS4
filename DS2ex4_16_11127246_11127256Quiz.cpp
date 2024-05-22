//11127246 陳柏劭 11127256 蕭天語
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <stack>
#include <random>
#include <chrono>
using namespace std;

// 邊的結構體，表示圖中的一條邊
struct Edge {
    int getIndex;  // 接收者索引
    float weight;  // 邊的權重
    string getID;  // 接收者學號
};

// 頂點的結構體，表示圖中的一個頂點
struct Vertex {
    string putID;            // 發送者學號
    vector<Edge> edges;      // 相鄰邊的串列
};

// 圖的類，包含所有頂點和邊的結構
class Graph {
private:
    vector<Vertex> vertices;          // 存儲所有頂點的串列
    unordered_map<string, int> idMap; // 學號到索引的映射
    int node = 0;                     // 節點數量

public:
    // 根據學號找頂點索引
    int findVertexIndex(const string& id) {
        unordered_map<string, int>::iterator it = idMap.find(id);
        if (it != idMap.end()) {
            return it->second;  // 返回頂點索引
        }
        return -1;  // 找不到返回 -1
    }

    // 從二進位文件讀取圖數據
    bool readBinary(const string& filename) {
        ifstream infile(filename, ios::binary); // 打開二進位文件
        if (!infile.is_open()) {
            return false;  // 無法打開文件
        }
        node = 0;
        vertices.clear(); // 清空頂點串列
        idMap.clear(); // 清空映射
        char putID[12], getID[12];
        float weight;

        while (infile.read(putID, 12)) { // 讀取發送者學號
            infile.read(getID, 12); // 讀取接收者學號
            infile.read(reinterpret_cast<char*>(&weight), sizeof(float)); // 讀取邊的權重

            string putIDStr(putID, 12);
            string getIDStr(getID, 12);
            putIDStr = putIDStr.c_str(); // 確保字符串以空字元終止
            getIDStr = getIDStr.c_str(); // 確保字符串以空字元終止

            int putIndex = findVertexIndex(putIDStr);
            if (putIndex == -1) { // 如果發送者不在頂點串列中
                putIndex = vertices.size();
                vertices.push_back({putIDStr, {}}); // 添加新的頂點
                idMap[putIDStr] = putIndex; // 更新映射
            }
            int getIndex = findVertexIndex(getIDStr);
            if (getIndex == -1) { // 如果接收者不在頂點串列中
                getIndex = vertices.size();
                vertices.push_back({getIDStr, {}}); // 添加新的頂點
                idMap[getIDStr] = getIndex; // 更新映射
            }
            vertices[putIndex].edges.push_back({getIndex, weight, getIDStr}); // 添加邊
            node++; // 增加節點數量
        }

        for (int i = 0; i < vertices.size(); i++) {
            sortEdges(vertices[i].edges); // 排序每個頂點的邊
        }
        infile.close(); // 關閉文件
        return true;
    }

    // 將圖的數據寫入文件
    void writeFile(const string& filename) {
        ofstream outfile(filename); // 打開輸出文件
        if (!outfile.is_open()) {
            return;
        }

        vector<Vertex> copy = vertices; // 複製頂點串列
        sortVertices(); // 排序頂點串列

        outfile << "<<< There are " << vertices.size() << " IDs in total. >>>\n";  // 輸出頂點總數
        cout << "\n<<< There are " << vertices.size() << " IDs in total. >>>\n";  // 輸出頂點總數
        for (int i = 0; i < vertices.size(); i++) {
            outfile << "[" << setw(3) << i + 1 << "] " << vertices[i].putID << ":\n";  // 輸出頂點學號
            for (int j = 0; j < vertices[i].edges.size(); j++) {
                ostringstream oss;
                oss << vertices[i].edges[j].weight;  // 將權重轉為字符串
                if ((j + 1) % 12 == 1 || (j + 1) % 12 == 11) 
                    outfile << "\t";
                else
                    outfile << "   ";
                outfile << "(" << setw(2) << j + 1 << ") " << vertices[i].edges[j].getID << ",   " << right << setw(4) << fixed << oss.str();  // 輸出邊的接收者學號和權重
                if ((j + 1) % 12 == 0) {
                    outfile << "\n";
                } 
            }
            outfile << "\n";
        }

        vertices = copy; // 恢復頂點串列
        outfile << "<<< There are " << node << " nodes in total. >>>\n"; // 輸出節點總數
        cout << "\n<<< There are " << node << " nodes in total. >>>\n"; // 輸出節點總數
        outfile.close(); // 關閉文件
    }

    // 比較兩個頂點，用於排序
    bool compareVertices(Vertex a, Vertex b) {
        return a.putID < b.putID;
    }

    // 比較兩個邊，用於排序
    bool compareEdges(Edge a, Edge b) {
        return a.getID < b.getID;
    }

    // 排序頂點
    void sortVertices() {
        int n = vertices.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (compareVertices(vertices[j + 1], vertices[j])) {
                    swap(vertices[j], vertices[j + 1]);
                }
            }
        }
    }

    // 排序邊
    void sortEdges(vector<Edge>& edges) {
        int n = edges.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (compareEdges(edges[j + 1], edges[j])) {
                    swap(edges[j], edges[j + 1]);
                }
            }
        }
    }

    // 寬度優先搜尋 (BFS)
    int bfs(int startIndex, vector<string>& visitedNodes) {
        vector<bool> visited(vertices.size(), false); // 記錄每個頂點是否已被訪問
        queue<int> q; // 用於 BFS 的佇列
        q.push(startIndex);
        visited[startIndex] = true;
        int count = 0;

        while (!q.empty()) {
            int u = q.front(); // 取得佇列的頭部節點
            q.pop();

            for (int i = 0; i < vertices[u].edges.size(); i++) {
                int neighborIndex = vertices[u].edges[i].getIndex; // 取得鄰接頂點的索引
                if (!visited[neighborIndex]) {
                    q.push(neighborIndex); // 將鄰接頂點加入佇列
                    visited[neighborIndex] = true; // 標記鄰接頂點為已訪問
                    visitedNodes.push_back(vertices[neighborIndex].putID); // 記錄已訪問頂點的學號
                    count++;
                }
            }
        }
        sort(visitedNodes.begin(), visitedNodes.end()); // 排序已訪問頂點
        return count; // 返回已訪問頂點的數量
    }

    // 計算連通數
    void connectionCounts(const string& filename) {
        vector<string> ids; // 存儲所有頂點的學號
        vector<int> counts; // 存儲每個頂點的連通數
        vector<vector<string>> visitedNodesList; // 存儲每個頂點的已訪問節點學號

        for (int i = 0; i < vertices.size(); i++) {
            vector<string> visitedNodes; // 用於存儲 BFS 過程中訪問的頂點
            int count = bfs(i, visitedNodes); // 執行 BFS 並取得連通數
            ids.push_back(vertices[i].putID); // 記錄學號
            counts.push_back(count); // 記錄連通數
            visitedNodesList.push_back(visitedNodes); // 記錄已訪問頂點
        }

        // 排序連通數和已訪問頂點
        for (int i = 0; i < ids.size() - 1; i++) {
            for (int j = 0; j < ids.size() - i - 1; j++) {
                if (counts[j] < counts[j + 1] || (counts[j] == counts[j + 1] && ids[j] > ids[j + 1])) {
                    swap(counts[j], counts[j + 1]);
                    swap(ids[j], ids[j + 1]);
                    swap(visitedNodesList[j], visitedNodesList[j + 1]);
                }
            }
        }

        ofstream outfile(filename); // 打開輸出文件
        if (!outfile.is_open()) {
            return;
        }

        outfile << "<<< There are " << vertices.size() << " IDs in total. >>>\n"; // 輸出頂點總數
        cout << "\n<<< There are " << vertices.size() << " IDs in total. >>>\n"; // 輸出頂點總數
        for (int i = 0; i < ids.size(); i++) {
            outfile << "[" << setw(3) << i + 1 << "] " << ids[i] << "(" << counts[i] << "):\n"; // 輸出學號和連通數
            for (int j = 0; j < visitedNodesList[i].size(); j++) {
                if ((j + 1) % 12 == 1) {
                    outfile << "        "; // 每第12項後開始新行並留8格
                } 
				else {
                    outfile << right << setw(3) << fixed;
                }
                outfile << "(" << setw(2) << j + 1 << ") " << right << setw(4) << visitedNodesList[i][j];
                if ((j + 1) % 12 == 0) {
                    outfile << "\n";
                } 
				else {
                    outfile << " ";
                }
            }
            outfile << "\n";
        }
        outfile.close(); // 關閉文件
    }

    // 深度優先搜尋 (DFS) 計算影響力
    int dfs(int startIndex, float threshold, vector<int>& visitedNodes) {
        stack<int> s;// 用於DFS的堆疊
        s.push(startIndex);// 將起始頂點加入堆疊
        vector<bool> visited(vertices.size(), false);// 訪問標記初始化為false
        visited[startIndex] = true;// 標記起始頂點為已訪問
        int count = 0;// 計數影響力節點數量

        while (!s.empty()) {
            int u = s.top();
            s.pop();

            for (int i = 0; i < vertices[u].edges.size(); i++) {
                int neighborIndex = vertices[u].edges[i].getIndex;
                if (vertices[u].edges[i].weight >= threshold && !visited[neighborIndex]) {// 如果邊的權重大於等於門檻且鄰接頂點未被訪問
                    s.push(neighborIndex);// 將鄰接頂點加入堆疊
                    visited[neighborIndex] = true;// 標記鄰接頂點為已訪問
                    visitedNodes.push_back(neighborIndex);// 記錄已訪問的頂點
                    count++;// 增加影響力節點數量
                }
            }
        }

        sort(visitedNodes.begin(), visitedNodes.end());
        return count;// 返回影響力節點數量
    }

    void sortInfluenceNodes(vector<int>& influenceNodes) {    // 排序影響力節點

	    for (int i = 0; i < influenceNodes.size() - 1; i++) {
	        for (int j = 0; j < influenceNodes.size() - i - 1; j++) {
	            if (vertices[influenceNodes[j]].putID > vertices[influenceNodes[j + 1]].putID) {
	                swap(influenceNodes[j], influenceNodes[j + 1]);
	            }
	        }
	    }
	}

	// 計算影響力
	void influence(const string& filename, float threshold) {
	    vector<string> ids;// 學號串列
	    vector<int> influenceCounts;// 影響力數量串列
	    vector<vector<int>> influenceNodesList;// 影響力節點串列
	
		// 遍歷每個頂點以計算影響力
	    for (int i = 0; i < vertices.size(); i++) {
	        vector<int> visitedNodes;
	        int influence = dfs(i, threshold, visitedNodes);// 執行DFS計算影響力
	        if (influence != 0) {
	            ids.push_back(vertices[i].putID);// 記錄學號
	            influenceCounts.push_back(influence);// 記錄影響力數量
	            influenceNodesList.push_back(visitedNodes);// 記錄影響力節點
	        }
	    }
	
	    // 排序影響力和已訪問頂點
	    for (int i = 0; i < ids.size() - 1; i++) {
	        for (int j = 0; j < ids.size() - i - 1; j++) {
	            if (influenceCounts[j] < influenceCounts[j + 1] || (influenceCounts[j] == influenceCounts[j + 1] && ids[j] > ids[j + 1])) {
	                swap(influenceCounts[j], influenceCounts[j + 1]);
	                swap(ids[j], ids[j + 1]);
	                swap(influenceNodesList[j], influenceNodesList[j + 1]);
	            }
	        }
	    }
	
	    ofstream outfile(filename);
	    if (!outfile.is_open()) {
	        return;
	    }
	
	    outfile << "<<< There are " << ids.size() << " IDs in total. >>>\n";
	    cout << "\n<<< There are " << ids.size() << " IDs in total. >>>\n";
	
	    for (int i = 0; i < ids.size(); i++) {
	        outfile << "[" << setw(3) << i + 1 << "] " << ids[i] << "(" << influenceCounts[i] << "):\n"; // 輸出學號和影響力
	        int count = 1;
	
	        // 手動排序影響力節點
	        sortInfluenceNodes(influenceNodesList[i]);
	
	        for (int j = 0; j < influenceNodesList[i].size(); j++) {
	            int index = influenceNodesList[i][j];
	            if (count % 12 == 1) {
	                outfile << "        "; // 每第12項後開始新行並留8格
	            } 
				else {
	                outfile << right << setw(3) << fixed;
	            }
	            outfile << "(" << setw(2) << count << ") " << right << setw(4) << vertices[index].putID;
	            if (count % 12 == 0) {
	                outfile << "\n";
	            } 
				else {
	                outfile << " ";
	            }
	            count++;
	        }
	        outfile << "\n";
	    }
	    outfile.close();
	}


     void topKInfluence() {
        vector<string> ids;// 學號串列
        vector<int> influenceCounts;// 影響力數量串列
        vector<vector<int>> influenceNodesList;// 影響力節點串列
        srand(time(0)); // 初始化隨機數生成器
		using namespace chrono;
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < vertices.size(); i++) {
            float threshold = 0.5 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.5)); // 隨機生成 0.5 到 1.0 之間的門檻
            vector<int> visitedNodes;
            int influence = dfs(i, threshold, visitedNodes);// 執行DFS計算影響力
            if (influence != 0) {
	            ids.push_back(vertices[i].putID);// 記錄學號
	            influenceCounts.push_back(influence);// 記錄影響力數量
	            influenceNodesList.push_back(visitedNodes); // 記錄影響力節點
	    	}
        }

		auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        // 排序影響力和已訪問頂點
        for (int i = 0; i < ids.size() - 1; i++) {
            for (int j = 0; j < ids.size() - i - 1; j++) {
                if (influenceCounts[j] < influenceCounts[j + 1] || (influenceCounts[j] == influenceCounts[j + 1] && ids[j] > ids[j + 1])) {
                    swap(influenceCounts[j], influenceCounts[j + 1]);
                    swap(ids[j], ids[j + 1]);
                    swap(influenceNodesList[j], influenceNodesList[j + 1]);
                }
            }
        }

    
        cout << "\n[Elapsed time] " << duration.count() << " ms" << endl;
		int k;
		while (true) {
	        cout << "\nInput an integer to show top-K in [1," << ids.size() << "]: ";	        
	        cin >> k;
	        if( k > ids.size() || k < 1)
	        	cout << "\n### " << k << " is NOT in [1," << ids.size() << "] ###" << endl;
	        else {
				cout << endl;
	        	break;
	    	}
		}

        for (int i = 0; i < k; i++) {
            cout << "<" << i + 1 << "> " << ids[i] << ": " << influenceCounts[i] << endl;
        }
    }
};


int main() {
    Graph graph; // 創建 Graph
    int command;
    string fileName;
    double command1done = 0;
    string copy;

    do {
        cout << "\n**** Graph data manipulation *****" << endl;
        cout << "* 0. QUIT                        *" << endl;
        cout << "* 1. Build adjacency lists       *" << endl;
        cout << "* 2. Compute connection counts   *" << endl;
        cout << "* 3. Estimate influence values   *" << endl;
        cout << "* 4. Probability-based influence *" << endl;
        cout << "**********************************" << endl;
        cout << "Input a choice(0, 1, 2, 3, 4): ";

        cin >> command;
        if (command == 0) {
            break;
        } 
		else if (command == 1) {
            cout << "\nInput a file number ([0] Quit): ";
            cin >> fileName;
            if (fileName.compare("0")) { // 如果檔案名不是 0 則讀取二進位檔案
                copy = fileName;
                fileName = "pairs" + fileName + ".bin";
                if (graph.readBinary(fileName)) { // 讀檔
                    fileName = "pairs" + copy + ".adj";
                    graph.writeFile(fileName); // 寫檔
                    command1done++;
                } 
				else {
                    cout << "\n### pairs" << copy << ".bin does not exist! ###" << endl;
                    command1done = 0;
                }
            } 
			else {
                command1done = 0;
            }
        } 
		else if (command == 2) {
            if (command1done == 0)
                cout << "### There is no graph and choose 1 first. ###" << endl;
            else if (fileName.compare("0")) {
                fileName = "pairs" + copy + ".cnt";
                graph.connectionCounts(fileName); // 計算連通數
            }
        } 
		else if (command == 3) {
            if (command1done == 0)
                cout << "### There is no graph and choose 1 first. ###" << endl;
            else if (fileName.compare("0")) {
                float threshold;
                while (true) {
                    cout << "\nInput a real number in [0,1]: ";
                    cin >> threshold;							//門檻
                    if (threshold < 0 || threshold > 1)
                        cout << "\n### " << threshold << " is NOT in [0,1] ###" << endl;
                    else
                        break;
                }
                fileName = "pairs" + copy + ".inf";
                graph.influence(fileName, threshold); // 計算影響力
            }
        } 
		else if (command == 4) {
            if (command1done == 0)
                cout << "### There is no graph and choose 1 first. ###" << endl;
            else if (fileName.compare("0")) {
                graph.topKInfluence(); // 計算前 K 名影響力
            }
        } 
		else {
            cout << endl << "Command does not exist!" << endl << endl;
        }

    } while (command != 0);
    return 0;
}