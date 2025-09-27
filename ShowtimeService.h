#ifndef SHOWTIMESERVICE_H
#define SHOWTIMESERVICE_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

// Auditorium class
class Auditorium {
private:
    int id;
    string name;
    string seat_map_json;
    int capacity;
    vector<string> format_support;
    string room_type; // Standard, IMAX, 4DX, etc.

public:
    Auditorium();
    Auditorium(int auditoriumId, const string& auditoriumName, int auditoriumCapacity);
    
    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    string getSeatMap() const { return seat_map_json; }
    int getCapacity() const { return capacity; }
    vector<string> getFormatSupport() const { return format_support; }
    string getRoomType() const { return room_type; }
    
    // Setters
    void setId(int newId) { id = newId; }
    void setName(const string& newName) { name = newName; }
    void setSeatMap(const string& newSeatMap) { seat_map_json = newSeatMap; }
    void setCapacity(int newCapacity) { capacity = newCapacity; }
    void setFormatSupport(const vector<string>& newFormats) { format_support = newFormats; }
    void setRoomType(const string& newType) { room_type = newType; }
    
    void displayInfo() const;
    bool supportsFormat(const string& format) const;
};

// Showtime class
class Showtime {
private:
    int id;
    int movie_version_id;
    int auditorium_id;
    time_t start_time;
    time_t end_time;
    int price_template_id;
    int seats_total;
    int seats_available;
    string status; // scheduled, canceled, completed
    int hold_timeout_seconds;
    string format; // 2D, 3D, IMAX, etc.
    double base_price;

public:
    Showtime();
    Showtime(int movieVersionId, int auditoriumId, time_t startTime, time_t endTime);
    
    // Getters
    int getId() const { return id; }
    int getMovieVersionId() const { return movie_version_id; }
    int getAuditoriumId() const { return auditorium_id; }
    time_t getStartTime() const { return start_time; }
    time_t getEndTime() const { return end_time; }
    int getPriceTemplateId() const { return price_template_id; }
    int getSeatsTotal() const { return seats_total; }
    int getSeatsAvailable() const { return seats_available; }
    string getStatus() const { return status; }
    int getHoldTimeout() const { return hold_timeout_seconds; }
    string getFormat() const { return format; }
    double getBasePrice() const { return base_price; }
    
    // Setters
    void setId(int newId) { id = newId; }
    void setMovieVersionId(int newMovieVersionId) { movie_version_id = newMovieVersionId; }
    void setAuditoriumId(int newAuditoriumId) { auditorium_id = newAuditoriumId; }
    void setStartTime(time_t newStartTime) { start_time = newStartTime; }
    void setEndTime(time_t newEndTime) { end_time = newEndTime; }
    void setPriceTemplateId(int newPriceTemplateId) { price_template_id = newPriceTemplateId; }
    void setSeatsTotal(int newSeatsTotal) { seats_total = newSeatsTotal; }
    void setSeatsAvailable(int newSeatsAvailable) { seats_available = newSeatsAvailable; }
    void setStatus(const string& newStatus) { status = newStatus; }
    void setHoldTimeout(int newTimeout) { hold_timeout_seconds = newTimeout; }
    void setFormat(const string& newFormat) { format = newFormat; }
    void setBasePrice(double newPrice) { base_price = newPrice; }
    
    void displayInfo() const;
    bool isValid() const;
    double getOccupancyRate() const;
};

// ShowtimeService class - Business Logic Layer
class ShowtimeService {
private:
    vector<Showtime> showtimes;
    vector<Auditorium> auditoriums;
    int nextShowtimeId;
    int nextAuditoriumId;
    
    bool validateShowtime(const Showtime& showtime) const;
    bool checkTimeConflict(int auditoriumId, time_t startTime, time_t endTime, int excludeShowtimeId = -1) const;
    vector<Showtime> heapSortShowtimes(vector<Showtime> showtimeList, bool byTime = true) const;

public:
    ShowtimeService();
    
    // Auditorium management
    bool createAuditorium(const Auditorium& auditorium);
    Auditorium* findAuditoriumById(int auditoriumId);
    vector<Auditorium> getAllAuditoriums() const;
    
    // Showtime CRUD operations
    bool createShowtime(const Showtime& showtime);
    bool updateShowtime(int showtimeId, const Showtime& updatedShowtime);
    bool cancelShowtime(int showtimeId, const string& reason = "");
    Showtime* findShowtimeById(int showtimeId);
    vector<Showtime> searchShowtimes(const string& query) const;
    vector<Showtime> filterShowtimes(const string& status = "", int auditoriumId = 0, 
                                   time_t fromDate = 0, time_t toDate = 0) const;
    
    // Bulk operations
    bool bulkCreateShowtimes(const vector<Showtime>& showtimeList);
    bool copySchedule(time_t fromDate, time_t toDate);
    
    // Conflict checking
    bool hasConflict(int auditoriumId, time_t startTime, time_t endTime) const;
    vector<Showtime> getConflictingShowtimes(int auditoriumId, time_t startTime, time_t endTime) const;
    
    // Statistics and reporting
    vector<Showtime> getShowtimesByDate(time_t date) const;
    vector<Showtime> getShowtimesByAuditorium(int auditoriumId) const;
    double getAverageOccupancyRate() const;
    vector<Showtime> getTopPerformingShowtimes(int limit = 10) const;
    
    // Demo functions for terminal UI
    void createShowtimeDemo();
    void updateShowtimeDemo();
    void cancelShowtimeDemo();
    void searchShowtimesDemo();
    void bulkCreateDemo();
    void copyScheduleDemo();
    
    // Utility
    void displayAllShowtimes() const;
    void displayAllAuditoriums() const;
    string formatTime(time_t timeValue) const;
    time_t parseTime(const string& timeStr) const;
};

#endif
