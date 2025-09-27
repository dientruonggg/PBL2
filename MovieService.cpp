#include "MovieService.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// Movie class implementation
Movie::Movie() : Entity(), duration_min(0), release_date(0) {
    status = "active";
}

Movie::Movie(const string& title, int duration, const string& rating) 
    : Entity(), title(title), duration_min(duration), rating_age(rating) {
    status = "active";
    release_date = time(0);
    slug = title; // Simplified slug generation
    transform(slug.begin(), slug.end(), slug.begin(), ::tolower);
    replace(slug.begin(), slug.end(), ' ', '-');
}

bool Movie::isValid() const {
    return !title.empty() && duration_min > 0 && !rating_age.empty();
}

void Movie::displayInfo() const {
    cout << "ID: " << id << " | Title: " << title << " | Duration: " << duration_min 
         << " min | Rating: " << rating_age << " | Status: " << status << endl;
}

// MovieVersion class implementation
MovieVersion::MovieVersion() : Entity(), movie_id(0), runtime(0) {}

MovieVersion::MovieVersion(int movieId, const string& versionType, int versionRuntime)
    : Entity(), movie_id(movieId), type(versionType), runtime(versionRuntime) {}

// MovieService class implementation
MovieService::MovieService() : nextMovieId(1) {
    // Initialize with some sample data
    Movie movie1("Aquaman", 143, "PG-13");
    movie1.setId(nextMovieId++);
    movie1.setGenres({"Action", "Adventure", "Fantasy"});
    movie1.setLanguage("English");
    movies.push_back(movie1);
    
    Movie movie2("Spider-Man", 121, "PG-13");
    movie2.setId(nextMovieId++);
    movie2.setGenres({"Action", "Adventure", "Sci-Fi"});
    movie2.setLanguage("English");
    movies.push_back(movie2);
    
    Movie movie3("The Batman", 176, "PG-13");
    movie3.setId(nextMovieId++);
    movie3.setGenres({"Action", "Crime", "Drama"});
    movie3.setLanguage("English");
    movies.push_back(movie3);
}

bool MovieService::validateMovie(const Movie& movie) const {
    if (movie.getTitle().empty()) {
        cout << "Error: Movie title cannot be empty!" << endl;
        return false;
    }
    if (movie.getDuration() <= 0) {
        cout << "Error: Movie duration must be positive!" << endl;
        return false;
    }
    if (movie.getRating().empty()) {
        cout << "Error: Movie rating cannot be empty!" << endl;
        return false;
    }
    return true;
}

string MovieService::generateSlug(const string& title) const {
    string slug = title;
    transform(slug.begin(), slug.end(), slug.begin(), ::tolower);
    replace(slug.begin(), slug.end(), ' ', '-');
    return slug;
}

// Heap sort implementation for movies (by rating or other criteria)
vector<Movie> MovieService::heapSortMovies(vector<Movie> movieList, bool byRating) const {
    // Simple heap sort implementation
    make_heap(movieList.begin(), movieList.end(), [byRating](const Movie& a, const Movie& b) {
        if (byRating) {
            return a.getRating() < b.getRating(); // Sort by rating
        }
        return a.getTitle() < b.getTitle(); // Sort by title
    });
    
    sort_heap(movieList.begin(), movieList.end(), [byRating](const Movie& a, const Movie& b) {
        if (byRating) {
            return a.getRating() < b.getRating();
        }
        return a.getTitle() < b.getTitle();
    });
    
    return movieList;
}

bool MovieService::createMovie(const Movie& movie) {
    if (!validateMovie(movie)) {
        return false;
    }
    
    Movie newMovie = movie;
    newMovie.setId(nextMovieId++);
    newMovie.setSlug(generateSlug(movie.getTitle()));
    movies.push_back(newMovie);
    
    cout << "Movie created successfully with ID: " << newMovie.getId() << endl;
    return true;
}

bool MovieService::updateMovie(int movieId, const Movie& updatedMovie) {
    Movie* movie = findMovieById(movieId);
    if (!movie) {
        cout << "Error: Movie with ID " << movieId << " not found!" << endl;
        return false;
    }
    
    if (!validateMovie(updatedMovie)) {
        return false;
    }
    
    // Check if trying to set inactive status while having active showtimes
    if (updatedMovie.getStatus() == "inactive" && hasActiveShowtimes(movieId)) {
        cout << "Error: Cannot set movie to inactive while it has active showtimes!" << endl;
        return false;
    }
    
    *movie = updatedMovie;
    movie->setId(movieId); // Preserve original ID
    movie->updateTimestamp();
    
    cout << "Movie updated successfully!" << endl;
    return true;
}

bool MovieService::archiveMovie(int movieId) {
    Movie* movie = findMovieById(movieId);
    if (!movie) {
        cout << "Error: Movie with ID " << movieId << " not found!" << endl;
        return false;
    }
    
    if (hasActiveShowtimes(movieId)) {
        cout << "Error: Cannot archive movie while it has future showtimes!" << endl;
        return false;
    }
    
    movie->setStatus("archived");
    movie->updateTimestamp();
    
    cout << "Movie archived successfully!" << endl;
    return true;
}

Movie* MovieService::findMovieById(int movieId) {
    for (auto& movie : movies) {
        if (movie.getId() == movieId) {
            return &movie;
        }
    }
    return nullptr;
}

// Fuzzy search implementation - finds best matches even with typos
vector<Movie> MovieService::searchMovies(const string& query) const {
    vector<Movie> results;
    vector<pair<Movie, int>> scoredResults;
    
    for (const auto& movie : movies) {
        int score = 0;
        string title = movie.getTitle();
        transform(title.begin(), title.end(), title.begin(), ::tolower);
        string lowerQuery = query;
        transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        // Exact match gets highest score
        if (title == lowerQuery) {
            score = 100;
        }
        // Contains query gets high score
        else if (title.find(lowerQuery) != string::npos) {
            score = 80;
        }
        // Fuzzy matching - check for similar characters
        else {
            int matches = 0;
            for (char c : lowerQuery) {
                if (title.find(c) != string::npos) {
                    matches++;
                }
            }
            score = (matches * 60) / lowerQuery.length();
        }
        
        if (score > 30) { // Threshold for relevance
            scoredResults.push_back({movie, score});
        }
    }
    
    // Sort by score (highest first)
    sort(scoredResults.begin(), scoredResults.end(), 
         [](const pair<Movie, int>& a, const pair<Movie, int>& b) {
             return a.second > b.second;
         });
    
    // Return top 10 results
    for (size_t i = 0; i < min(scoredResults.size(), size_t(10)); ++i) {
        results.push_back(scoredResults[i].first);
    }
    
    return results;
}

vector<Movie> MovieService::filterMovies(const string& status, const string& genre, 
                                        const string& rating, int year) const {
    vector<Movie> results;
    
    for (const auto& movie : movies) {
        bool matches = true;
        
        if (!status.empty() && movie.getStatus() != status) {
            matches = false;
        }
        if (!genre.empty()) {
            auto genres = movie.getGenres();
            bool hasGenre = find(genres.begin(), genres.end(), genre) != genres.end();
            if (!hasGenre) matches = false;
        }
        if (!rating.empty() && movie.getRating() != rating) {
            matches = false;
        }
        // Year filtering would require parsing release_date
        
        if (matches) {
            results.push_back(movie);
        }
    }
    
    return results;
}

bool MovieService::bulkImportMovies(const vector<Movie>& movieList) {
    int successCount = 0;
    for (const auto& movie : movieList) {
        if (createMovie(movie)) {
            successCount++;
        }
    }
    
    cout << "Bulk import completed: " << successCount << "/" << movieList.size() 
         << " movies imported successfully." << endl;
    return successCount > 0;
}

int MovieService::getActiveMovieCount() const {
    int count = 0;
    for (const auto& movie : movies) {
        if (movie.getStatus() == "active") {
            count++;
        }
    }
    return count;
}

vector<Movie> MovieService::getTopRatedMovies(int limit) const {
    vector<Movie> activeMovies = filterMovies("active");
    vector<Movie> sortedMovies = heapSortMovies(activeMovies, true);
    
    if (sortedMovies.size() > static_cast<size_t>(limit)) {
        sortedMovies.resize(limit);
    }
    
    return sortedMovies;
}

vector<Movie> MovieService::getMoviesByGenre(const string& genre) const {
    return filterMovies("", genre);
}

bool MovieService::hasActiveShowtimes(int movieId) const {
    // This would check with ShowtimeService in a real implementation
    // For demo purposes, return false
    return false;
}

void MovieService::displayAllMovies() const {
    cout << "\n=== ALL MOVIES ===" << endl;
    for (const auto& movie : movies) {
        movie.displayInfo();
    }
}

// Demo functions for terminal UI
void MovieService::createMovieDemo() {
    cout << "\n=== CREATE MOVIE ===" << endl;
    string title, rating, language;
    int duration;
    
    cout << "Enter movie title: ";
    cin.ignore();
    getline(cin, title);
    
    cout << "Enter duration (minutes): ";
    cin >> duration;
    
    cout << "Enter rating (G/PG/PG-13/R): ";
    cin >> rating;
    
    cout << "Enter language: ";
    cin >> language;
    
    Movie newMovie(title, duration, rating);
    newMovie.setLanguage(language);
    
    if (createMovie(newMovie)) {
        cout << "Movie created successfully!" << endl;
    }
}

void MovieService::updateMovieDemo() {
    cout << "\n=== UPDATE MOVIE ===" << endl;
    displayAllMovies();
    
    int movieId;
    cout << "Enter movie ID to update: ";
    cin >> movieId;
    
    Movie* movie = findMovieById(movieId);
    if (!movie) {
        cout << "Movie not found!" << endl;
        return;
    }
    
    cout << "Current movie info:" << endl;
    movie->displayInfo();
    
    cout << "Enter new title (or press enter to keep current): ";
    cin.ignore();
    string newTitle;
    getline(cin, newTitle);
    
    if (!newTitle.empty()) {
        movie->setTitle(newTitle);
        cout << "Movie updated successfully!" << endl;
    }
}

void MovieService::searchMoviesDemo() {
    cout << "\n=== SEARCH MOVIES ===" << endl;
    cout << "Enter search query (try 'aquafina' to find 'Aquaman'): ";
    
    string query;
    cin.ignore();
    getline(cin, query);
    
    vector<Movie> results = searchMovies(query);
    
    if (results.empty()) {
        cout << "No movies found matching: " << query << endl;
    } else {
        cout << "\nSearch results for '" << query << "':" << endl;
        for (const auto& movie : results) {
            movie.displayInfo();
        }
    }
}

void MovieService::archiveMovieDemo() {
    cout << "\n=== ARCHIVE MOVIE ===" << endl;
    displayAllMovies();
    
    int movieId;
    cout << "Enter movie ID to archive: ";
    cin >> movieId;
    
    archiveMovie(movieId);
}

void MovieService::bulkImportDemo() {
    cout << "\n=== BULK IMPORT DEMO ===" << endl;
    
    vector<Movie> bulkMovies;
    bulkMovies.push_back(Movie("Avatar", 162, "PG-13"));
    bulkMovies.push_back(Movie("Titanic", 194, "PG-13"));
    bulkMovies.push_back(Movie("Inception", 148, "PG-13"));
    
    cout << "Importing 3 sample movies..." << endl;
    bulkImportMovies(bulkMovies);
}

void MovieService::showStatisticsDemo() {
    cout << "\n=== MOVIE STATISTICS ===" << endl;
    cout << "Active movies: " << getActiveMovieCount() << endl;
    
    cout << "\nTop rated movies:" << endl;
    vector<Movie> topMovies = getTopRatedMovies(5);
    for (const auto& movie : topMovies) {
        movie.displayInfo();
    }
}
