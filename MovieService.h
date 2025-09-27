#ifndef MOVIESERVICE_H
#define MOVIESERVICE_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

// Base Entity class
class Entity {
protected:
    int id;
    time_t created_at;
    time_t updated_at;
    
public:
    Entity() : id(0), created_at(time(0)), updated_at(time(0)) {}
    virtual ~Entity() {}
    
    int getId() const { return id; }
    void setId(int newId) { id = newId; }
    time_t getCreatedAt() const { return created_at; }
    time_t getUpdatedAt() const { return updated_at; }
    void updateTimestamp() { updated_at = time(0); }
};

// Movie class inheriting from Entity
class Movie : public Entity {
private:
    string title;
    string original_title;
    string slug;
    string synopsis;
    int duration_min;
    string rating_age;
    string language;
    vector<string> genres;
    string poster_url;
    string trailer_url;
    string status;
    time_t release_date;
    string created_by;

public:
    Movie();
    Movie(const string& title, int duration, const string& rating);
    
    // Getters
    string getTitle() const { return title; }
    string getOriginalTitle() const { return original_title; }
    string getSlug() const { return slug; }
    string getSynopsis() const { return synopsis; }
    int getDuration() const { return duration_min; }
    string getRating() const { return rating_age; }
    string getLanguage() const { return language; }
    vector<string> getGenres() const { return genres; }
    string getPosterUrl() const { return poster_url; }
    string getTrailerUrl() const { return trailer_url; }
    string getStatus() const { return status; }
    time_t getReleaseDate() const { return release_date; }
    string getCreatedBy() const { return created_by; }
    
    // Setters
    void setTitle(const string& newTitle) { title = newTitle; updateTimestamp(); }
    void setOriginalTitle(const string& newOriginalTitle) { original_title = newOriginalTitle; }
    void setSlug(const string& newSlug) { slug = newSlug; }
    void setSynopsis(const string& newSynopsis) { synopsis = newSynopsis; }
    void setDuration(int newDuration) { duration_min = newDuration; updateTimestamp(); }
    void setRating(const string& newRating) { rating_age = newRating; }
    void setLanguage(const string& newLanguage) { language = newLanguage; }
    void setGenres(const vector<string>& newGenres) { genres = newGenres; }
    void setPosterUrl(const string& newPosterUrl) { poster_url = newPosterUrl; }
    void setTrailerUrl(const string& newTrailerUrl) { trailer_url = newTrailerUrl; }
    void setStatus(const string& newStatus) { status = newStatus; updateTimestamp(); }
    void setReleaseDate(time_t newReleaseDate) { release_date = newReleaseDate; }
    void setCreatedBy(const string& newCreatedBy) { created_by = newCreatedBy; }
    
    // Validation
    bool isValid() const;
    void displayInfo() const;
};

// MovieVersion class for different formats (2D, 3D, IMAX, etc.)
class MovieVersion : public Entity {
private:
    int movie_id;
    string type;
    int runtime;
    vector<string> subtitles;
    vector<string> format_flags;

public:
    MovieVersion();
    MovieVersion(int movieId, const string& versionType, int versionRuntime);
    
    // Getters
    int getMovieId() const { return movie_id; }
    string getType() const { return type; }
    int getRuntime() const { return runtime; }
    vector<string> getSubtitles() const { return subtitles; }
    vector<string> getFormatFlags() const { return format_flags; }
    
    // Setters
    void setMovieId(int newMovieId) { movie_id = newMovieId; }
    void setType(const string& newType) { type = newType; }
    void setRuntime(int newRuntime) { runtime = newRuntime; }
    void setSubtitles(const vector<string>& newSubtitles) { subtitles = newSubtitles; }
    void setFormatFlags(const vector<string>& newFlags) { format_flags = newFlags; }
};

// MovieService class - Business Logic Layer
class MovieService {
private:
    vector<Movie> movies;
    vector<MovieVersion> movieVersions;
    int nextMovieId;
    
    bool validateMovie(const Movie& movie) const;
    string generateSlug(const string& title) const;
    vector<Movie> heapSortMovies(vector<Movie> movieList, bool byRating = false) const;
    
public:
    MovieService();
    
    // Core CRUD operations
    bool createMovie(const Movie& movie);
    bool updateMovie(int movieId, const Movie& updatedMovie);
    bool archiveMovie(int movieId);
    Movie* findMovieById(int movieId);
    vector<Movie> searchMovies(const string& query) const;
    vector<Movie> filterMovies(const string& status = "", const string& genre = "", 
                              const string& rating = "", int year = 0) const;
    
    // Bulk operations
    bool bulkImportMovies(const vector<Movie>& movieList);
    
    // Statistics
    int getActiveMovieCount() const;
    vector<Movie> getTopRatedMovies(int limit = 10) const;
    vector<Movie> getMoviesByGenre(const string& genre) const;
    
    // Demo functions for terminal UI
    void createMovieDemo();
    void updateMovieDemo();
    void searchMoviesDemo();
    void archiveMovieDemo();
    void bulkImportDemo();
    void showStatisticsDemo();
    
    // Utility
    void displayAllMovies() const;
    bool hasActiveShowtimes(int movieId) const; // Check if movie has future showtimes
};

#endif
