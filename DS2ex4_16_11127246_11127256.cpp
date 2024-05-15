//11127246 陳柏劭 11127256 蕭天語
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <queue>

using namespace std;

// 邊結構 
struct Edge {
    string getID; // 接收者學號
    float weight; // 邊的權重
};

// 頂點結構 
struct Vertex {
    string putID; // 發送者學號
    vector<Edge> edges; // 相鄰邊的串列
};


class Graph {
private:
    vector<Vertex> vertices; // 存所有頂點的串列
    int node = 0; // 節點數量

    // 根據學號找頂點索引
    int findVertexIndex(string id) {
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i].putID == id) {
                return i;
            }
        }
        return -1;
    }

public:
    // 從二進位文件讀取圖數據
    bool readBinary(string filename) {
        ifstream infile(filename, ios::binary); // 打開二進位文件
        if (!infile.is_open()) {
            return false;
        }
        node = 0;
        vector<Vertex>().swap(vertices); // 清空頂點串列
        char putID[12], getID[12]; // 宣告 12 個字元的陣列
        float weight;

        while (infile.read(putID, 12)) {
            infile.read(getID, 12);
            infile.read((char*)&weight, sizeof(float));

            string putIDStr(putID, 12);  
            string getIDStr(getID, 12);  
            putIDStr = putIDStr.c_str(); // 確保字符串以空字元終止
            getIDStr = getIDStr.c_str(); // 確保字符串以空字元終止

            Edge edge = {getIDStr, weight}; // 創建邊
            node++;
            int index = findVertexIndex(putIDStr); // 找發送者頂點索引
            if (index == -1) {
                vertices.push_back({putIDStr, vector<Edge>{edge}}); // 如果不存在，創建新的頂點
            } 
	    else {
                vertices[index].edges.push_back(edge); // 如果存在，添加邊到現有頂點
            }
        }
        for (int i = 0; i < vertices.size(); i++) {
            sortEdges(vertices[i].edges); // 排序邊
        }

        sortVertices(); // 排序頂點
        infile.close(); // 關閉文件
        return true;
    }

    // 將圖的數據寫入文件
    void writeFile(string filename) {
        ofstream outfile(filename); // 打開文件
        if (!outfile.is_open()) {
            return;
        }

        outfile << "<<< There are " << vertices.size() << " IDs in total. >>>\n";
        cout << "\n<<< There are " << vertices.size() << " IDs in total. >>>\n";
        for (int i = 0; i < vertices.size(); i++) {
            outfile << "[" << i + 1 << "] " << vertices[i].putID << ":\n";
            for (int j = 0; j < vertices[i].edges.size(); j++) {
                ostringstream oss;
                oss << vertices[i].edges[j].weight;
                outfile << "(" << j + 1 << ") " << vertices[i].edges[j].getID << ", "
                     << fixed << setprecision(2) << oss.str() ;
                if (( j + 1 )% 12 != 0)
                	outfile << " ";
                else if (( j + 1 )% 12 == 0 && j != 0)
                	outfile << endl;
            }
            outfile << "\n";
        }

        outfile << "<<< There are " << node << " nodes in total. >>>\n";
        cout << "\n<<< There are " << node << " nodes in total. >>>\n";
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

    // 排序連接節點
    void sortConnections(vector<string>& connections) {
        int n = connections.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (connections[j] > connections[j + 1]) {
                    swap(connections[j], connections[j + 1]);
                }
            }
        }
    }

    // 排序連通數
    void sortConnectionCounts(vector<string>& id, vector<int>& counts, vector<vector<string>>& visitedNodesList) {
        int n = id.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (counts[j] < counts[j + 1]) {
                    swap(counts[j], counts[j + 1]);
                    swap(id[j], id[j + 1]);
                    swap(visitedNodesList[j], visitedNodesList[j + 1]);
                } 
				else if (counts[j] == counts[j + 1] && id[j] > id[j + 1]) {
                    swap(counts[j], counts[j + 1]);
                    swap(id[j], id[j + 1]);
                    swap(visitedNodesList[j], visitedNodesList[j + 1]);
                }
            }
        }
    }

    // 寬度優先搜尋 (BFS)
    int bfs(string startID, vector<string>& visitedNodes) {
        vector<bool> visited(vertices.size(), false); // 頂點是否訪問串列
        queue<int> q; // 訪問佇列
        int startIndex = findVertexIndex(startID); // 起始頂點索引
        if (startIndex == -1)
            return 0;

        q.push(startIndex); // 將起始頂點加入佇列
        visited[startIndex] = true; // 標記起始頂點為已訪問
        int count = 0; // 計數器

        while (!q.empty()) {
            int u = q.front(); // 取得佇列的頭
            q.pop(); // 移除佇列的頭

            for (int i = 0; i < vertices[u].edges.size(); i++) {
                int adjacentIndex = findVertexIndex(vertices[u].edges[i].getID); // 取得鄰接頂點索引
                if (adjacentIndex != -1 && !visited[adjacentIndex]) {
                    q.push(adjacentIndex); // 將鄰接頂點加入佇列
                    visited[adjacentIndex] = true; // 標記鄰接頂點為已訪問
                    visitedNodes.push_back(vertices[adjacentIndex].putID); // 記錄已訪問頂點
                    count++;
                }
            }
        }
        sortConnections(visitedNodes); // 排序已訪問頂點
        return count; // 返回計數結果
    }

    // 計算連通數
    void connectionCounts(string filename) {
        vector<string> id; // 學號串列
        vector<int> counts; // 連通數串列
        vector<vector<string>> visitedNodesList; // 已訪問頂點串列

        for (int i = 0; i < vertices.size(); i++) {
            vector<string> visitedNodes; // 當前頂點的已訪問頂點
            int count = bfs(vertices[i].putID, visitedNodes); // 執行 BFS 並取得連通數
            id.push_back(vertices[i].putID); // 記錄學號
            counts.push_back(count); // 記錄連通數
            visitedNodesList.push_back(visitedNodes); // 記錄已訪問頂點
        }

        sortConnectionCounts(id, counts, visitedNodesList); // 排序連通數和已訪問頂點

        ofstream outfile(filename); // 打開輸出文件
        if (!outfile.is_open()) {
            return;
        }

        outfile << "<<< There are " << vertices.size() << " IDs in total. >>>\n";
        cout << "\n<<< There are " << vertices.size() << " IDs in total. >>>\n";
        for (int i = 0; i < id.size(); i++) {
            outfile << "[" << i + 1 << "] " << id[i] << "(" << counts[i] << "):\n";
            for (int j = 0; j < visitedNodesList[i].size(); j++) {
                outfile << "(" << j + 1 << ") " << visitedNodesList[i][j] ;
                if (( j + 1 )% 12 != 0)
                	outfile << " ";
                else if (( j + 1 )% 12 == 0 && j != 0)
                	outfile << endl;
            }
            outfile << endl;
        }
        outfile.close(); // 關閉文件
    }
};

int main() {
	string fileName; 	
    int command ;
    double command1done = 0;
    bool success = false;
    Graph graph;			//創建Graph
    string copy;
    
    do {

        cout << "\n**** Graph data manipulation *****" << endl;
        cout << "* 0. QUIT                        *" << endl;
        cout << "* 1. Build adjacency lists       *" << endl;
        cout << "* 2. Compute connection counts   *" << endl;
        cout << "**********************************" << endl;
        cout << "Input a choice(0, 1, 2): ";
        

        cin >> command;
        if( command == 0 ){
            break;
        }
        else if( command == 1 ){					
        	cout << "\nInput a file number ([0] Quit): "; 
            cin >> fileName;
            if (fileName.compare("0")) 	{		//if filename not 0 then readbin
            	copy = fileName;
            	fileName = "pairs" + fileName + ".bin";
            	if (graph.readBinary(fileName)){					//讀檔
            		fileName = "pairs" + copy + ".adj";
    				graph.writeFile(fileName);  					//寫檔
    				command1done++;
				}
				else{
					cout << "\n### pairs" << copy << ".bin does not exist! ###" << endl;
					command1done = 0; 
				}
            }
            else{
            	command1done = 0; 
            }
        }
        else if( command == 2 ){					
        	if ( command1done == 0 )
        		cout<< "### There is no graph and choose 1 first. ###" <<endl ;
        	else if (fileName.compare("0")) {		
				fileName = "pairs" + copy + ".cnt";
                graph.connectionCounts(fileName);				//計算連通數
        	}
        }      	
		else
            cout << endl << "Command does not exist!" << endl << endl;
        
    }
    while ( command  != 0 );
    system("pause");
    return 0;
}
