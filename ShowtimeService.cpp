#include "ShowtimeService.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>

// Auditorium class implementation
Auditorium::Auditorium() : id(0), capacity(0) {}

Auditorium::Auditorium(int auditoriumId, const string& auditoriumName, int auditoriumCapacity)
    : id(auditoriumId), name(auditoriumName), capacity(auditoriumCapacity) {
    room_type = "Standard";
    format_support = {"2D", "3D"};
}

void Auditorium::displayInfo() const {
    cout << "Auditorium ID: " << id << " | Name: " << name 
         << " | Capacity: " << capacity << " | Type: " << room_type << endl;
}

bool Auditorium::supportsFormat(const string& format) const {
    return find(format_support.begin(), format_support.end(), format) != format_support.end();
}

// Showtime class implementation
Showtime::Showtime() : id(0), movie_version_id(0), auditorium_id(0), 
                       start_time(0), end_time(0), price_template_id(0),
                       seats_total(0), seats_available(0), hold_timeout_seconds(300),
                       base_price(10.0) {
    status = "scheduled";
    format = "2D";
}

Showtime::Showtime(int movieVersionId, int auditoriumId, time_t startTime, time_t endTime)
    : id(0), movie_version_id(movieVersionId), auditorium_id(auditoriumId),
      start_time(startTime), end_time(endTime), price_template_id(0),
      seats_total(100), seats_available(100), hold_timeout_seconds(300),
      base_price(10.0) {
    status = "scheduled";
    format = "2D";
}

void Showtime::displayInfo() const {
    cout << "Showtime ID: " << id << " | Movie Version: " << movie_version_id
         << " | Auditorium: " << auditorium_id << " | Format: " << format
         << " | Available Seats: " << seats_available << "/" << seats_total
         << " | Status: " << status << " | Price: $" << base_price << endl;
    
    // Display time information
    char startBuffer[80], endBuffer[80];
    strftime(startBuffer, sizeof(startBuffer), "%Y-%m-%d %H:%M", localtime(&start_time));
    strftime(endBuffer, sizeof(endBuffer), "%Y-%m-%d %H:%M", localtime(&end_time));
    cout << "Start: " << startBuffer << " | End: " << endBuffer << endl;
}

bool Showtime::isValid() const {
    return movie_version_id > 0 && auditorium_id > 0 && 
           start_time > 0 && end_time > start_time && 
           seats_total > 0 && base_price > 0;
}

double Showtime::getOccupancyRate() const {
    if (seats_total == 0) return 0.0;
    return (double)(seats_total - seats_available) / seats_total * 100.0;
}

// ShowtimeService class implementation
ShowtimeService::ShowtimeService() : nextShowtimeId(1), nextAuditoriumId(1) {
    // Initialize with sample auditoriums
    Auditorium aud1(nextAuditoriumId++, "Theater 1", 100);
    aud1.setRoomType("Standard");
    auditoriums.push_back(aud1);
    
    Auditorium aud2(nextAuditoriumId++, "IMAX Theater", 150);
    aud2.setRoomType("IMAX");
    aud2.setFormatSupport({"2D", "3D", "IMAX"});
    auditoriums.push_back(aud2);
    
    Auditorium aud3(nextAuditoriumId++, "4DX Theater", 80);
    aud3.setRoomType("4DX");
    aud3.setFormatSupport({"2D", "3D", "4DX"});
    auditoriums.push_back(aud3);
    
    // Initialize with sample showtimes
    time_t now = time(0);
    time_t tomorrow = now + 24 * 3600;
    
    Showtime show1(1, 1, tomorrow + 3600, tomorrow + 5400); // 1 hour from tomorrow, 2.5 hours duration
    show1.setId(nextShowtimeId++);
    show1.setFormat("2D");
    show1.setBasePrice(12.0);
    show1.setSeatsTotal(100);
    show1.setSeatsAvailable(85);
    showtimes.push_back(show1);
    
    Showtime show2(1, 2, tomorrow + 7200, tomorrow + 9600); // 2 hours from tomorrow
    show2.setId(nextShowtimeId++);
    show2.setFormat("IMAX");
    show2.setBasePrice(18.0);
    show2.setSeatsTotal(150);
    show2.setSeatsAvailable(120);
    showtimes.push_back(show2);
}

bool ShowtimeService::validateShowtime(const Showtime& showtime) const {
    if (!showtime.isValid()) {
        cout << "Error: Invalid showtime data!" << endl;
        return false;
    }
    
    // Check if auditorium exists
    bool auditoriumExists = false;
    for (const auto& aud : auditoriums) {
        if (aud.getId() == showtime.getAuditoriumId()) {
            auditoriumExists = true;
            // Check if auditorium supports the format
            if (!aud.supportsFormat(showtime.getFormat())) {
                cout << "Error: Auditorium does not support format: " << showtime.getFormat() << endl;
                return false;
            }
            break;
        }
    }
    
    if (!auditoriumExists) {
        cout << "Error: Auditorium with ID " << showtime.getAuditoriumId() << " does not exist!" << endl;
        return false;
    }
    
    // Check if start time is in the future
    time_t now = time(0);
    if (showtime.getStartTime() <= now) {
        cout << "Error: Cannot create showtime in the past!" << endl;
        return false;
    }
    
    return true;
}

bool ShowtimeService::checkTimeConflict(int auditoriumId, time_t startTime, time_t endTime, int excludeShowtimeId) const {
    for (const auto& showtime : showtimes) {
        if (showtime.getAuditoriumId() == auditoriumId && 
            showtime.getId() != excludeShowtimeId &&
            showtime.getStatus() != "canceled") {
            
            // Check for overlap with 30-minute buffer
            time_t bufferTime = 30 * 60; // 30 minutes
            time_t existingStart = showtime.getStartTime() - bufferTime;
            time_t existingEnd = showtime.getEndTime() + bufferTime;
            
            if ((startTime >= existingStart && startTime < existingEnd) ||
                (endTime > existingStart && endTime <= existingEnd) ||
                (startTime <= existingStart && endTime >= existingEnd)) {
                return true; // Conflict found
            }
        }
    }
    return false; // No conflict
}

// Heap sort implementation for showtimes
vector<Showtime> ShowtimeService::heapSortShowtimes(vector<Showtime> showtimeList, bool byTime) const {
    make_heap(showtimeList.begin(), showtimeList.end(), [byTime](const Showtime& a, const Showtime& b) {
        if (byTime) {
            return a.getStartTime() > b.getStartTime(); // Sort by start time (earliest first)
        }
        return a.getOccupancyRate() < b.getOccupancyRate(); // Sort by occupancy rate (highest first)
    });
    
    sort_heap(showtimeList.begin(), showtimeList.end(), [byTime](const Showtime& a, const Showtime& b) {
        if (byTime) {
            return a.getStartTime() > b.getStartTime();
        }
        return a.getOccupancyRate() < b.getOccupancyRate();
    });
    
    return showtimeList;
}

bool ShowtimeService::createAuditorium(const Auditorium& auditorium) {
    Auditorium newAuditorium = auditorium;
    newAuditorium.setId(nextAuditoriumId++);
    auditoriums.push_back(newAuditorium);
    
    cout << "Auditorium created successfully with ID: " << newAuditorium.getId() << endl;
    return true;
}

Auditorium* ShowtimeService::findAuditoriumById(int auditoriumId) {
    for (auto& auditorium : auditoriums) {
        if (auditorium.getId() == auditoriumId) {
            return &auditorium;
        }
    }
    return nullptr;
}

vector<Auditorium> ShowtimeService::getAllAuditoriums() const {
    return auditoriums;
}

bool ShowtimeService::createShowtime(const Showtime& showtime) {
    if (!validateShowtime(showtime)) {
        return false;
    }
    
    // Check for conflicts
    if (checkTimeConflict(showtime.getAuditoriumId(), showtime.getStartTime(), showtime.getEndTime())) {
        cout << "Error: Time conflict detected with existing showtime!" << endl;
        return false;
    }
    
    Showtime newShowtime = showtime;
    newShowtime.setId(nextShowtimeId++);
    
    // Set seats total based on auditorium capacity
    Auditorium* aud = findAuditoriumById(showtime.getAuditoriumId());
    if (aud) {
        newShowtime.setSeatsTotal(aud->getCapacity());
        newShowtime.setSeatsAvailable(aud->getCapacity());
    }
    
    showtimes.push_back(newShowtime);
    
    cout << "Showtime created successfully with ID: " << newShowtime.getId() << endl;
    return true;
}

bool ShowtimeService::updateShowtime(int showtimeId, const Showtime& updatedShowtime) {
    Showtime* showtime = findShowtimeById(showtimeId);
    if (!showtime) {
        cout << "Error: Showtime with ID " << showtimeId << " not found!" << endl;
        return false;
    }
    
    // Check if showtime has sold tickets
    if (showtime->getSeatsAvailable() < showtime->getSeatsTotal()) {
        cout << "Warning: This showtime has sold tickets. Limited updates allowed." << endl;
        
        // Only allow certain updates when tickets are sold
        showtime->setBasePrice(updatedShowtime.getBasePrice());
        showtime->setFormat(updatedShowtime.getFormat());
        
        cout << "Showtime updated with limited changes!" << endl;
        return true;
    }
    
    if (!validateShowtime(updatedShowtime)) {
        return false;
    }
    
    // Check for conflicts (excluding current showtime)
    if (checkTimeConflict(updatedShowtime.getAuditoriumId(), 
                         updatedShowtime.getStartTime(), 
                         updatedShowtime.getEndTime(), 
                         showtimeId)) {
        cout << "Error: Time conflict detected with existing showtime!" << endl;
        return false;
    }
    
    *showtime = updatedShowtime;
    showtime->setId(showtimeId); // Preserve original ID
    
    cout << "Showtime updated successfully!" << endl;
    return true;
}

bool ShowtimeService::cancelShowtime(int showtimeId, const string& reason) {
    Showtime* showtime = findShowtimeById(showtimeId);
    if (!showtime) {
        cout << "Error: Showtime with ID " << showtimeId << " not found!" << endl;
        return false;
    }
    
    if (showtime->getSeatsAvailable() < showtime->getSeatsTotal()) {
        cout << "Warning: This showtime has sold tickets. Cancellation requires refund processing." << endl;
        cout << "Reason for cancellation: " << reason << endl;
    }
    
    showtime->setStatus("canceled");
    
    cout << "Showtime canceled successfully!" << endl;
    return true;
}

Showtime* ShowtimeService::findShowtimeById(int showtimeId) {
    for (auto& showtime : showtimes) {
        if (showtime.getId() == showtimeId) {
            return &showtime;
        }
    }
    return nullptr;
}

vector<Showtime> ShowtimeService::searchShowtimes(const string& query) const {
    vector<Showtime> results;
    
    // Try to parse as showtime ID
    try {
        int showtimeId = stoi(query);
        for (const auto& showtime : showtimes) {
            if (showtime.getId() == showtimeId) {
                results.push_back(showtime);
                return results;
            }
        }
    } catch (...) {
        // Not a number, continue with other search methods
    }
    
    // Search by format or status
    for (const auto& showtime : showtimes) {
        if (showtime.getFormat().find(query) != string::npos ||
            showtime.getStatus().find(query) != string::npos) {
            results.push_back(showtime);
        }
    }
    
    return results;
}

vector<Showtime> ShowtimeService::filterShowtimes(const string& status, int auditoriumId, 
                                                 time_t fromDate, time_t toDate) const {
    vector<Showtime> results;
    
    for (const auto& showtime : showtimes) {
        bool matches = true;
        
        if (!status.empty() && showtime.getStatus() != status) {
            matches = false;
        }
        if (auditoriumId > 0 && showtime.getAuditoriumId() != auditoriumId) {
            matches = false;
        }
        if (fromDate > 0 && showtime.getStartTime() < fromDate) {
            matches = false;
        }
        if (toDate > 0 && showtime.getStartTime() > toDate) {
            matches = false;
        }
        
        if (matches) {
            results.push_back(showtime);
        }
    }
    
    return results;
}

bool ShowtimeService::bulkCreateShowtimes(const vector<Showtime>& showtimeList) {
    int successCount = 0;
    for (const auto& showtime : showtimeList) {
        if (createShowtime(showtime)) {
            successCount++;
        }
    }
    
    cout << "Bulk creation completed: " << successCount << "/" << showtimeList.size() 
         << " showtimes created successfully." << endl;
    return successCount > 0;
}

bool ShowtimeService::copySchedule(time_t fromDate, time_t toDate) {
    vector<Showtime> sourceShowtimes = getShowtimesByDate(fromDate);
    vector<Showtime> newShowtimes;
    
    time_t dateDiff = toDate - fromDate;
    
    for (const auto& showtime : sourceShowtimes) {
        Showtime newShowtime = showtime;
        newShowtime.setId(0); // Will be assigned new ID
        newShowtime.setStartTime(showtime.getStartTime() + dateDiff);
        newShowtime.setEndTime(showtime.getEndTime() + dateDiff);
        newShowtime.setSeatsAvailable(showtime.getSeatsTotal()); // Reset availability
        newShowtimes.push_back(newShowtime);
    }
    
    return bulkCreateShowtimes(newShowtimes);
}

bool ShowtimeService::hasConflict(int auditoriumId, time_t startTime, time_t endTime) const {
    return checkTimeConflict(auditoriumId, startTime, endTime);
}

vector<Showtime> ShowtimeService::getConflictingShowtimes(int auditoriumId, time_t startTime, time_t endTime) const {
    vector<Showtime> conflicts;
    
    for (const auto& showtime : showtimes) {
        if (showtime.getAuditoriumId() == auditoriumId && 
            showtime.getStatus() != "canceled") {
            
            if ((startTime >= showtime.getStartTime() && startTime < showtime.getEndTime()) ||
                (endTime > showtime.getStartTime() && endTime <= showtime.getEndTime()) ||
                (startTime <= showtime.getStartTime() && endTime >= showtime.getEndTime())) {
                conflicts.push_back(showtime);
            }
        }
    }
    
    return conflicts;
}

vector<Showtime> ShowtimeService::getShowtimesByDate(time_t date) const {
    vector<Showtime> results;
    
    // Get start and end of the day
    struct tm* dateInfo = localtime(&date);
    dateInfo->tm_hour = 0;
    dateInfo->tm_min = 0;
    dateInfo->tm_sec = 0;
    time_t dayStart = mktime(dateInfo);
    time_t dayEnd = dayStart + 24 * 3600;
    
    for (const auto& showtime : showtimes) {
        if (showtime.getStartTime() >= dayStart && showtime.getStartTime() < dayEnd) {
            results.push_back(showtime);
        }
    }
    
    return results;
}

vector<Showtime> ShowtimeService::getShowtimesByAuditorium(int auditoriumId) const {
    return filterShowtimes("", auditoriumId);
}

double ShowtimeService::getAverageOccupancyRate() const {
    if (showtimes.empty()) return 0.0;
    
    double totalOccupancy = 0.0;
    for (const auto& showtime : showtimes) {
        totalOccupancy += showtime.getOccupancyRate();
    }
    
    return totalOccupancy / showtimes.size();
}

vector<Showtime> ShowtimeService::getTopPerformingShowtimes(int limit) const {
    vector<Showtime> sortedShowtimes = heapSortShowtimes(showtimes, false); // Sort by occupancy
    
    if (sortedShowtimes.size() > static_cast<size_t>(limit)) {
        sortedShowtimes.resize(limit);
    }
    
    return sortedShowtimes;
}

void ShowtimeService::displayAllShowtimes() const {
    cout << "\n=== ALL SHOWTIMES ===" << endl;
    for (const auto& showtime : showtimes) {
        showtime.displayInfo();
        cout << "---" << endl;
    }
}

void ShowtimeService::displayAllAuditoriums() const {
    cout << "\n=== ALL AUDITORIUMS ===" << endl;
    for (const auto& auditorium : auditoriums) {
        auditorium.displayInfo();
    }
}

string ShowtimeService::formatTime(time_t timeValue) const {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&timeValue));
    return string(buffer);
}

time_t ShowtimeService::parseTime(const string& timeStr) const {
    // Simple time parsing - in real implementation would use proper parsing
    return time(0) + 3600; // Return 1 hour from now as placeholder
}

// Demo functions
void ShowtimeService::createShowtimeDemo() {
    cout << "\n=== CREATE SHOWTIME ===" << endl;
    displayAllAuditoriums();
    
    int movieVersionId, auditoriumId;
    string format;
    double price;
    
    cout << "Enter movie version ID: ";
    cin >> movieVersionId;
    
    cout << "Enter auditorium ID: ";
    cin >> auditoriumId;
    
    cout << "Enter format (2D/3D/IMAX/4DX): ";
    cin >> format;
    
    cout << "Enter base price: $";
    cin >> price;
    
    // Create showtime for tomorrow
    time_t tomorrow = time(0) + 24 * 3600;
    time_t endTime = tomorrow + 2 * 3600; // 2 hours duration
    
    Showtime newShowtime(movieVersionId, auditoriumId, tomorrow, endTime);
    newShowtime.setFormat(format);
    newShowtime.setBasePrice(price);
    
    createShowtime(newShowtime);
}

void ShowtimeService::updateShowtimeDemo() {
    cout << "\n=== UPDATE SHOWTIME ===" << endl;
    displayAllShowtimes();
    
    int showtimeId;
    cout << "Enter showtime ID to update: ";
    cin >> showtimeId;
    
    Showtime* showtime = findShowtimeById(showtimeId);
    if (!showtime) {
        cout << "Showtime not found!" << endl;
        return;
    }
    
    cout << "Current showtime info:" << endl;
    showtime->displayInfo();
    
    double newPrice;
    cout << "Enter new price (current: $" << showtime->getBasePrice() << "): $";
    cin >> newPrice;
    
    showtime->setBasePrice(newPrice);
    cout << "Showtime updated successfully!" << endl;
}

void ShowtimeService::cancelShowtimeDemo() {
    cout << "\n=== CANCEL SHOWTIME ===" << endl;
    displayAllShowtimes();
    
    int showtimeId;
    cout << "Enter showtime ID to cancel: ";
    cin >> showtimeId;
    
    string reason;
    cout << "Enter cancellation reason: ";
    cin.ignore();
    getline(cin, reason);
    
    cancelShowtime(showtimeId, reason);
}

void ShowtimeService::searchShowtimesDemo() {
    cout << "\n=== SEARCH SHOWTIMES ===" << endl;
    cout << "Enter search query (ID, format, or status): ";
    
    string query;
    cin.ignore();
    getline(cin, query);
    
    vector<Showtime> results = searchShowtimes(query);
    
    if (results.empty()) {
        cout << "No showtimes found matching: " << query << endl;
    } else {
        cout << "\nSearch results for '" << query << "':" << endl;
        for (const auto& showtime : results) {
            showtime.displayInfo();
            cout << "---" << endl;
        }
    }
}

void ShowtimeService::bulkCreateDemo() {
    cout << "\n=== BULK CREATE DEMO ===" << endl;
    
    vector<Showtime> bulkShowtimes;
    time_t baseTime = time(0) + 24 * 3600; // Tomorrow
    
    // Create 3 sample showtimes
    for (int i = 0; i < 3; i++) {
        time_t startTime = baseTime + i * 4 * 3600; // 4 hours apart
        time_t endTime = startTime + 2 * 3600; // 2 hours duration
        
        Showtime showtime(1, 1, startTime, endTime);
        showtime.setFormat("2D");
        showtime.setBasePrice(12.0 + i * 2); // Varying prices
        bulkShowtimes.push_back(showtime);
    }
    
    cout << "Creating 3 sample showtimes..." << endl;
    bulkCreateShowtimes(bulkShowtimes);
}

void ShowtimeService::copyScheduleDemo() {
    cout << "\n=== COPY SCHEDULE DEMO ===" << endl;
    
    time_t today = time(0);
    time_t tomorrow = today + 24 * 3600;
    time_t dayAfterTomorrow = today + 48 * 3600;
    
    cout << "Copying schedule from tomorrow to day after tomorrow..." << endl;
    
    if (copySchedule(tomorrow, dayAfterTomorrow)) {
        cout << "Schedule copied successfully!" << endl;
    } else {
        cout << "Failed to copy schedule!" << endl;
    }
}
