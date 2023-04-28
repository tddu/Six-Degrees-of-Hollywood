#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <queue>
#include <chrono>
#include "common.h"
using namespace std;


vector<int> bellmanFord(int actor1Index, int actor2Index, const vector<vector<vector<string>>> &adjMatrix) {
  bool pathcheck = false;
  queue<int> q;
  vector<bool> visit(adjMatrix.size(), false);
  vector<int> dist(adjMatrix.size(), INT_MAX);
  vector<int> prev(adjMatrix.size(), -1);

  visit[actor1Index] = true;
  dist[actor1Index] = 0;
  q.push(actor1Index);

  while (!q.empty()) {
    int k = q.front();
    q.pop();
    for (int i = 0; i < adjMatrix[k].size(); i++) {
      if (!adjMatrix[k][i].empty()) {
        if (!visit[i]) {
          visit[i] = true;
          dist[i] = dist[k] + 1;
          prev[i] = k;
          q.push(i);

          if (i == actor2Index) {
            pathcheck = true;
          }
        }
      }
    }
  }

  if (pathcheck) {
    //if a path exists print it out
    vector<int> path;
    int j = actor2Index;
    path.push_back(j);
    while (prev[j] != -1) {
      path.push_back(prev[j]);
      j = prev[j];
    }
    return path;
  } else {
    return vector<int>{};
  }
}
vector<int> dijkstra(int actor1Index, int actor2Index, const vector<vector<vector<string>>>& adjMatrix) {
  unordered_set<int> visited; //Set of Visited
  visited.emplace(actor1Index); //Begin with Start Node
  auto greaterPair = [](const pair<int, vector<int>>& a, const pair<int, vector<int>>& b){return a.second.size() > b.second.size();}; //Lambda greater. Path distance encoded in vector length
  priority_queue<pair<int, vector<int>>, vector<pair<int, vector<int>>>, decltype(greaterPair)> minimumDistances{greaterPair}; //https://stackoverflow.com/questions/5807735/c-priority-queue-with-lambda-comparator-error
  for (int i = 0; i < adjMatrix[actor1Index].size(); i++){
    if (visited.find(i) == visited.end() && !adjMatrix[actor1Index][i].empty()){ //If not visited, and share a move, there's a vertex
      vector<int> newVec = {actor1Index , i};
      minimumDistances.emplace(i , newVec);
    }
  }
  while(!minimumDistances.empty()){ //If no more vertices to traverse
    auto [movie, path] = minimumDistances.top();
    minimumDistances.pop();
    if(visited.find(movie) != visited.end()){ //Don't double check already visited
      continue;
    }
    if(movie == actor2Index){ //If shortest path to destination
      reverse(path.begin(), path.end());
      return path;
    }
    visited.emplace(movie); //Add node to visited
    for (int i = 0; i < adjMatrix[movie].size(); i++){ //Add all vertices
      if (visited.find(i) == visited.end() && !adjMatrix[movie][i].empty()){
        vector<int> newVec = path;
        newVec.push_back(i);
        minimumDistances.emplace(i , newVec);
      }
    }
  }
return vector<int>{}; //If no path, return empty.
}
int main() {


  cout << "Loading...";
  //actors is an unordered_set of actors, moviesToactors is an unordered_map that unordered_maps movies to their actors, actorsToindex is an unordered_map of an actor to their index
  unordered_set<Actor, Actor::actor_hash> actors;
  unordered_map<Movie, vector<Actor>, Movie::movie_hash> moviesToactors;
  unordered_map<string, int> actorsToindex;
  unordered_map<int, Actor> indexToactors;
  //got timer idea from stackoverflow.com/questions/2808398/easily-measure-elapsed-time


  //read csv file and fill in unordered_maps and unordered_set
  ifstream file("actorfilms.csv");
  string line;
  getline(file, line);
  while (getline(file, line)) {
    string actor;
    string movie{};
    stringstream sl(line);
    string word;
    vector<string> splitLine;
    while (getline(sl, word, ',')) {
      splitLine.push_back(word);
    }
    for (auto iter = splitLine.begin() + 2; iter != (splitLine.rbegin() + 4).base(); iter++) {
      movie.append(*iter);
    }
    Movie newMovie{movie, stoi((*splitLine.rbegin()).substr(2))};
    Actor newActor{splitLine[0], stoi(splitLine[1].substr(2))};
    actors.emplace(newActor);

    moviesToactors[newMovie].push_back(newActor);
  }
  //generate the actorsToindex unordered_map
  int p = 0;
  for (const auto & actor : actors) {
    actorsToindex[actor.name] = p;
    indexToactors[p] = actor;
    p++;
  }

  //generate adjacency matrix
  vector<vector<vector<string>>> adjMatrix(actors.size(), vector<vector<string>>(actors.size(), vector<string>()));
  for (auto & moviesToactor : moviesToactors) {
    for (int j = 0; j < moviesToactor.second.size(); j++) {
      for (int k = 0; k < moviesToactor.second.size(); k++) {
        if (j != k) {
          adjMatrix[actorsToindex[(moviesToactor.second[j]).name]][actorsToindex[(moviesToactor.second[k]).name]].push_back((moviesToactor.first).name);
        }
      }
    }
  }
  std::cout << "\x1B[2J\x1B[H"; //https://stackoverflow.com/questions/6486289/how-can-i-clear-console
  cout << "Welcome to This? Find out what separates your favorite actors!\n";

  //start of app
  while (true) {
    //take input
    string actor1, actor2;
    cout  << endl << "Enter name of first actor: " << endl;
    getline(cin, actor1);
    if (actorsToindex.find(actor1) == actorsToindex.end()) {
      cout << actor1 << " is not in the graph." << endl;
      continue;
    }
    bool validActor = false;
    while (validActor == false) {
      cout << "Enter name of second actor: " << endl;
      getline(cin, actor2);
      if (actorsToindex.find(actor2) == actorsToindex.end()) {
        cout << actor2 << " is not in the graph." << endl;
      }
      else {
        validActor = true;
      }
    }
    //membership check
    /*if (actorsToindex.find(actor1) == actorsToindex.end() || actorsToindex.find(actor2) == actorsToindex.end()) {
      cout << "One or both of the actors you entered are not in the graph." << endl;
      continue;
    }*/

    //check if they have worked together

    auto start =  std::chrono::steady_clock::now();
    auto end =  std::chrono::steady_clock::now();
    double secondCount = 0;

    if (adjMatrix[actorsToindex[actor1]][actorsToindex[actor2]].empty()) {
      //find shortest path if they haven't worked together
      //djikstra
      string algorithmString;
      vector<int> path;
      cout << "Pick an Algorithm: Dijkstra (1) or Bellman-Ford (2)" << endl;
      while (true) {
        bool validAlgo = false;
        getline(cin, algorithmString);
        if (algorithmString == "1" || algorithmString == "2") {
          validAlgo = true;
        }
        if (validAlgo == false) {
          cout << "Invalid choice. Please enter 1 for Dijkstra or 2 for Bellman-Ford" << endl;
        }
        else {break;}
      }
      if(algorithmString == "1") {
        //got timer idea from stackoverflow.com/questions/2808398/easily-measure-elapsed-time
        start =  std::chrono::steady_clock::now();
        path = dijkstra(actorsToindex[actor1], actorsToindex[actor2], adjMatrix);
        end = std::chrono::steady_clock::now();
        chrono::duration<double> time_span = std::chrono::duration_cast<chrono::duration<double>>(end - start);
        secondCount = time_span.count();
      } else if(algorithmString == "2"){
        start =  std::chrono::steady_clock::now();
        path = bellmanFord(actorsToindex[actor1], actorsToindex[actor2], adjMatrix);
        end = std::chrono::steady_clock::now();
        chrono::duration<double> time_span = std::chrono::duration_cast<chrono::duration<double>>(end - start);
        secondCount = time_span.count();
      } else {
        cout << "Invalid algorithm!" << endl;
        continue;
      }
      if (path.empty()) {
        cout << "There is no path between these actors." << endl;
      } else {
        cout << endl << actor1 << " and " << actor2 << " are seperated by " << path.size() - 2 << " actor(s):" << endl;

        int curr = path[path.size() - 1];
        for (int i = path.size() - 2; i >= 0; i--) {
          cout << indexToactors[curr].name << " acted in (";

          for (int j = 0; j < adjMatrix[curr][path[i]].size(); j++) {
            cout << adjMatrix[curr][path[i]][j];
            if (j != adjMatrix[curr][path[i]].size() - 1) {
              cout << ", ";
            }
          }
          cout << ") with " << indexToactors[path[i]].name << endl;
          curr = path[i];
        }
      }
    } else {
      //print out movies they costarred in
      cout << endl << "The actors " << actor1 << " and " << actor2
           << " have worked together in the following movie(s): " << endl;
      for (const auto & i : adjMatrix[actorsToindex[actor1]][actorsToindex[actor2]]) {
        cout << i << endl;
      }
      cout << endl;
    }
    cout<< "This algorithm took " << secondCount << " seconds!" << endl;
    cout << "Quit? [Y] or [N]" << endl;
    string quitter;
    bool valid = false;
    bool toQuit = false;
    while (valid == false) {
      getline(cin, quitter);
      if(quitter == "Y" || quitter == "y") {
        valid = true;
        toQuit = true;
        break;
      } 
      else if (quitter == "N" || quitter == "n") {
        valid = true;
        continue;
      }
      else {
        cout << "Invalid command. Please enter [Y] or [N]" << endl;
      }
    }
    if (toQuit == true) {
      break;
    }
  }
  return 0;
}