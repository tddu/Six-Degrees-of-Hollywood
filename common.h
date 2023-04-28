//
// Created by Adi on 4/25/2023.
//

#ifndef P3C__COMMON_H_
#define P3C__COMMON_H_
#include <string>
using std::string;
struct Actor { //https://stackoverflow.com/questions/38554083/how-can-i-use-a-c-unordered-set-for-a-custom-class
  string name;
  int id;
  bool operator==(const Actor& otherActor) const {
    return this->id == otherActor.id;
  }
  struct actor_hash {
    size_t operator()(const Actor& actor) const {
      return std::hash<int>()(actor.id);
    }
  };
  Actor() : id(0) {};
  Actor(const string& name, int id) : name(name), id(id) {};
};
struct Movie {
  string name;
  int id;
  bool operator==(const Movie& otherMovie) const {
    return this->id == otherMovie.id;
  }
  struct movie_hash {
    size_t operator()(const Movie& movie) const {
      return std::hash<int>()(movie.id);
    }
  };
  Movie(const string& name, int id) : name(name), id(id) {};
};
#endif //P3C__COMMON_H_
