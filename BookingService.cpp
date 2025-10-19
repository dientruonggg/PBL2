
#include "BookingService.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstring>

// Seat class implementation
Seat::Seat() : seat_id(""), row(""), number(0), type("Standard"), 
               status("available"), hold_expires_at(0), order_id(0), price_multiplier(1.0) {}

Seat::Seat(const string& seatId, const string& seatType) 
    : seat_id(seatId), type(seatType), status("available"), 
      hold_expires_at(0), order_id(0), price_multiplier(1.0) {
    setSeatId(seatId);
    
    // Set price multiplier based on seat type
    if (type == "VIP") {
        price_multiplier = 1.5;
    } else if (type == "Couple") {
        price_multiplier = 1.3;
    } else if (type == "Premium") {
        price_multiplier = 1.8;
    }
}

void Seat::setSeatId(const string& newSeatId) {
    seat_id = newSeatId;
    if (!newSeatId.empty()) {
        row = newSeatId.substr(0, 1);
        try {
            number = stoi(newSeatId.substr(1));
        } catch (...) {
            number = 0;
        }
    }
}

bool Seat::isAvailable() const {
    return status == "available";
}

bool Seat::isHeld() const {
    return status == "held" && !isHoldExpired();
}

bool Seat::isSold() const {
    return status == "sold";
}

bool Seat::isHoldExpired() const {
    return status == "held" && time(0) > hold_expires_at;
}

void Seat::displayInfo() const {
    cout << seat_id << "(" << type << "): " << status;
    if (status == "held") {
        cout << " [expires in " << (hold_expires_at - time(0)) << "s]";
    }
}

// Order class implementation
Order::Order() : id(0), staff_id(0), showtime_id(0), subtotal(0.0), tax(0.0), 
                 discount(0.0), total_amount(0.0), payment_status("pending"),
                 created_at(time(0)), updated_at(time(0)) {}

Order::Order(int staffId, int showtimeId, const vector<string>& seatIds)
    : id(0), staff_id(staffId), showtime_id(showtimeId), seat_ids(seatIds),
      subtotal(0.0), tax(0.0), discount(0.0), total_amount(0.0), 
      payment_status("pending"), created_at(time(0)), updated_at(time(0)) {}

void Order::calculateTotal() {
    total_amount = subtotal + tax - discount;
    updated_at = time(0);
}

void Order::displayInfo() const {
    cout << "Order ID: " << id << " | Staff: " << staff_id 
         << " | Showtime: " << showtime_id << " | Seats: ";
    for (const auto& seat : seat_ids) {
        cout << seat << " ";
    }
    cout << endl;
    cout << "Subtotal: $" << subtotal << " | Tax: $" << tax 
         << " | Discount: $" << discount << " | Total: $" << total_amount << endl;
    cout << "Status: " << payment_status << " | Customer: " << customer_name 
         << " | Phone: " << customer_phone << endl;
}

bool Order::isValid() const {
    return staff_id > 0 && showtime_id > 0 && !seat_ids.empty() && total_amount >= 0;
}

// Ticket class implementation
Ticket::Ticket() : ticket_id(""), order_id(0), showtime_id(0), seat_id(""),
                   show_time(0), price(0.0), status("valid"), issued_at(time(0)) {}

Ticket::Ticket(int orderId, int showtimeId, const string& seatId)
    : order_id(orderId), showtime_id(showtimeId), seat_id(seatId),
      show_time(0), price(0.0), status("valid"), issued_at(time(0)) {
    ticket_id = generateTicketId();
}

void Ticket::displayTicket() const {
    cout << "\n========== CINEMA TICKET ==========" << endl;
    cout << "Ticket ID: " << ticket_id << endl;
    cout << "Movie: " << movie_title << endl;
    cout << "Auditorium: " << auditorium_name << endl;
    cout << "Seat: " << seat_id << endl;
    
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&show_time));
    cout << "Show Time: " << timeBuffer << endl;
    
    cout << "Price: $" << fixed << setprecision(2) << price << endl;
    cout << "Status: " << status << endl;
    
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&issued_at));
    cout << "Issued: " << timeBuffer << endl;
    cout << "===================================" << endl;
}

string Ticket::generateTicketId() const {
    stringstream ss;
    ss << "TKT" << setfill('0') << setw(6) << (order_id * 1000 + rand() % 1000);
    return ss.str();
}

bool Ticket::isValid() const {
    return !ticket_id.empty() && order_id > 0 && showtime_id > 0 && 
           !seat_id.empty() && status == "valid";
}

// BookingService class implementation
BookingService::BookingService() : nextOrderId(1), nextTicketId(1) {
    // Initialize sample seat maps for showtimes
    initializeSeatsForShowtime(1, 100); // Showtime 1 with 100 seats
    initializeSeatsForShowtime(2, 150); // Showtime 2 with 150 seats
}

bool BookingService::validateSeatSelection(int showtimeId, const vector<string>& seatIds) const {
    if (seatIds.empty()) {
        cout << "Error: No seats selected!" << endl;
        return false;
    }
    
    // Check if showtime exists
    if (showtimeSeats.find(showtimeId) == showtimeSeats.end()) {
        cout << "Error: Showtime not found!" << endl;
        return false;
    }
    
    // Check if all seats are valid and available
    const vector<Seat>& seats = showtimeSeats.at(showtimeId);
    for (const string& seatId : seatIds) {
        bool found = false;
        for (const Seat& seat : seats) {
            if (seat.getSeatId() == seatId) {
                found = true;
                if (!seat.isAvailable()) {
                    cout << "Error: Seat " << seatId << " is not available!" << endl;
                    return false;
                }
                break;
            }
        }
        if (!found) {
            cout << "Error: Seat " << seatId << " does not exist!" << endl;
            return false;
        }
    }
    
    return true;
}

double BookingService::calculateSeatPrice(const string& seatId, double basePrice) const {
    // Find seat type and apply multiplier
    // This is a simplified implementation
    if (seatId.find("V") != string::npos) { // VIP seats contain 'V'
        return basePrice * 1.5;
    } else if (seatId.find("C") != string::npos) { // Couple seats contain 'C'
        return basePrice * 1.3;
    }
    return basePrice; // Standard price
}

void BookingService::initializeSeatsForShowtime(int showtimeId, int totalSeats) {
    vector<Seat> seats;
    
    // Generate seat layout (simplified)
    int seatsPerRow = 10;
    int rows = (totalSeats + seatsPerRow - 1) / seatsPerRow;
    
    for (int row = 0; row < rows; row++) {
        char rowLetter = 'A' + row;
        for (int seatNum = 1; seatNum <= seatsPerRow && seats.size() < totalSeats; seatNum++) {
            stringstream ss;
            ss << rowLetter << setfill('0') << setw(2) << seatNum;
            string seatId = ss.str();
            
            string seatType = "Standard";
            if (row >= rows - 2) { // Last 2 rows are VIP
                seatType = "VIP";
            } else if (seatNum == 5 || seatNum == 6) { // Middle seats are couple seats
                seatType = "Couple";
            }
            
            Seat seat(seatId, seatType);
            seats.push_back(seat);
        }
    }
    
    showtimeSeats[showtimeId] = seats;
}

// Heap sort implementation for seats
vector<Seat> BookingService::heapSortSeats(vector<Seat> seatList, bool byPrice) const {
    make_heap(seatList.begin(), seatList.end(), [byPrice](const Seat& a, const Seat& b) {
        if (byPrice) {
            return a.getPriceMultiplier() < b.getPriceMultiplier();
        }
        return a.getSeatId() > b.getSeatId(); // Sort by seat ID
    });
    
    sort_heap(seatList.begin(), seatList.end(), [byPrice](const Seat& a, const Seat& b) {
        if (byPrice) {
            return a.getPriceMultiplier() < b.getPriceMultiplier();
        }
        return a.getSeatId() > b.getSeatId();
    });
    
    return seatList;
}

vector<Seat> BookingService::getSeatsForShowtime(int showtimeId) {
    if (showtimeSeats.find(showtimeId) == showtimeSeats.end()) {
        initializeSeatsForShowtime(showtimeId, 100); // Default 100 seats
    }
    return showtimeSeats[showtimeId];
}

bool BookingService::holdSeats(int showtimeId, const vector<string>& seatIds, int holdTimeMinutes) {
    if (!validateSeatSelection(showtimeId, seatIds)) {
        return false;
    }
    
    vector<Seat>& seats = showtimeSeats[showtimeId];
    time_t holdExpiry = time(0) + holdTimeMinutes * 60;
    
    for (const string& seatId : seatIds) {
        for (Seat& seat : seats) {
            if (seat.getSeatId() == seatId) {
                seat.setStatus("held");
                seat.setHoldExpiresAt(holdExpiry);
                break;
            }
        }
    }
    
    cout << "Seats held for " << holdTimeMinutes << " minutes." << endl;
    return true;
}

bool BookingService::releaseHeldSeats(int showtimeId, const vector<string>& seatIds) {
    if (showtimeSeats.find(showtimeId) == showtimeSeats.end()) {
        return false;
    }
    
    vector<Seat>& seats = showtimeSeats[showtimeId];
    
    for (const string& seatId : seatIds) {
        for (Seat& seat : seats) {
            if (seat.getSeatId() == seatId && seat.getStatus() == "held") {
                seat.setStatus("available");
                seat.setHoldExpiresAt(0);
                seat.setOrderId(0);
                break;
            }
        }
    }
    
    cout << "Held seats released." << endl;
    return true;
}

void BookingService::releaseExpiredHolds() {
    for (auto& showtimePair : showtimeSeats) {
        vector<Seat>& seats = showtimePair.second;
        for (Seat& seat : seats) {
            if (seat.isHoldExpired()) {
                seat.setStatus("available");
                seat.setHoldExpiresAt(0);
                seat.setOrderId(0);
            }
        }
    }
}

bool BookingService::areSeatsAvailable(int showtimeId, const vector<string>& seatIds) const {
    return validateSeatSelection(showtimeId, seatIds);
}

bool BookingService::createOrder(const Order& order) {
    if (!order.isValid()) {
        cout << "Error: Invalid order data!" << endl;
        return false;
    }
    
    if (!validateSeatSelection(order.getShowtimeId(), order.getSeatIds())) {
        return false;
    }
    
    Order newOrder = order;
    newOrder.setId(nextOrderId++);
    orders.push_back(newOrder);
    
    cout << "Order created successfully with ID: " << newOrder.getId() << endl;
    return true;
}

bool BookingService::updateOrder(int orderId, const Order& updatedOrder) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order with ID " << orderId << " not found!" << endl;
        return false;
    }
    
    if (!updatedOrder.isValid()) {
        cout << "Error: Invalid order data!" << endl;
        return false;
    }
    
    *order = updatedOrder;
    order->setId(orderId); // Preserve original ID
    
    cout << "Order updated successfully!" << endl;
    return true;
}

Order* BookingService::findOrderById(int orderId) {
    for (auto& order : orders) {
        if (order.getId() == orderId) {
            return &order;
        }
    }
    return nullptr;
}

vector<Order> BookingService::getOrdersByStaff(int staffId) const {
    vector<Order> results;
    for (const auto& order : orders) {
        if (order.getStaffId() == staffId) {
            results.push_back(order);
        }
    }
    return results;
}

vector<Order> BookingService::getOrdersByShowtime(int showtimeId) const {
    vector<Order> results;
    for (const auto& order : orders) {
        if (order.getShowtimeId() == showtimeId) {
            results.push_back(order);
        }
    }
    return results;
}

bool BookingService::confirmBooking(int orderId) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order not found!" << endl;
        return false;
    }
    
    // Mark seats as sold
    vector<Seat>& seats = showtimeSeats[order->getShowtimeId()];
    for (const string& seatId : order->getSeatIds()) {
        for (Seat& seat : seats) {
            if (seat.getSeatId() == seatId) {
                seat.setStatus("sold");
                seat.setOrderId(orderId);
                break;
            }
        }
    }
    
    order->setPaymentStatus("paid");
    
    // Issue tickets
    issueTickets(orderId);
    
    cout << "Booking confirmed successfully!" << endl;
    return true;
}

bool BookingService::cancelBooking(int orderId, const string& reason) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order not found!" << endl;
        return false;
    }
    
    // Release seats
    releaseHeldSeats(order->getShowtimeId(), order->getSeatIds());
    
    order->setPaymentStatus("canceled");
    
    cout << "Booking canceled. Reason: " << reason << endl;
    return true;
}

bool BookingService::exchangeTicket(int orderId, int newShowtimeId, const vector<string>& newSeatIds) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order not found!" << endl;
        return false;
    }
    
    if (!validateSeatSelection(newShowtimeId, newSeatIds)) {
        return false;
    }
    
    // Release old seats
    releaseHeldSeats(order->getShowtimeId(), order->getSeatIds());
    
    // Hold new seats
    if (!holdSeats(newShowtimeId, newSeatIds)) {
        return false;
    }
    
    // Update order
    order->setShowtimeId(newShowtimeId);
    order->setSeatIds(newSeatIds);
    
    cout << "Ticket exchanged successfully!" << endl;
    return true;
}

bool BookingService::refundTicket(int orderId, const string& reason) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order not found!" << endl;
        return false;
    }
    
    // Release seats
    releaseHeldSeats(order->getShowtimeId(), order->getSeatIds());
    
    order->setPaymentStatus("refunded");
    
    // Mark tickets as canceled
    for (auto& ticket : tickets) {
        if (ticket.getOrderId() == orderId) {
            ticket.setStatus("canceled");
        }
    }
    
    cout << "Ticket refunded. Reason: " << reason << endl;
    return true;
}

bool BookingService::issueTickets(int orderId) {
    Order* order = findOrderById(orderId);
    if (!order) {
        cout << "Error: Order not found!" << endl;
        return false;
    }
    
    for (const string& seatId : order->getSeatIds()) {
        Ticket ticket(orderId, order->getShowtimeId(), seatId);
        ticket.setMovieTitle("Sample Movie"); // Would get from movie service
        ticket.setAuditoriumName("Theater 1"); // Would get from showtime service
        ticket.setShowTime(time(0) + 3600); // Sample show time
        ticket.setPrice(12.0); // Would calculate based on seat type
        
        tickets.push_back(ticket);
    }
    
    cout << "Tickets issued successfully!" << endl;
    return true;
}

vector<Ticket> BookingService::getTicketsByOrder(int orderId) const {
    vector<Ticket> results;
    for (const auto& ticket : tickets) {
        if (ticket.getOrderId() == orderId) {
            results.push_back(ticket);
        }
    }
    return results;
}

Ticket* BookingService::findTicketById(const string& ticketId) {
    for (auto& ticket : tickets) {
        if (ticket.getTicketId() == ticketId) {
            return &ticket;
        }
    }
    return nullptr;
}

bool BookingService::validateTicket(const string& ticketId) const {
    for (const auto& ticket : tickets) {
        if (ticket.getTicketId() == ticketId) {
            return ticket.isValid();
        }
    }
    return false;
}

double BookingService::calculateOrderTotal(int showtimeId, const vector<string>& seatIds, 
                                          double basePrice, double taxRate, double discount) const {
    double subtotal = 0.0;
    
    for (const string& seatId : seatIds) {
        subtotal += calculateSeatPrice(seatId, basePrice);
    }
    
    double tax = subtotal * taxRate;
    double total = subtotal + tax - discount;
    
    return total;
}

void BookingService::displaySeatMap(int showtimeId) const {
    if (showtimeSeats.find(showtimeId) == showtimeSeats.end()) {
        cout << "No seat map available for showtime " << showtimeId << endl;
        return;
    }
    
    const vector<Seat>& seats = showtimeSeats.at(showtimeId);
    
    cout << "\n=== SEAT MAP FOR SHOWTIME " << showtimeId << " ===" << endl;
    cout << "Legend: [A] Available, [H] Held, [X] Sold" << endl;
    cout << "        SCREEN" << endl;
    cout << "======================" << endl;
    
    string currentRow = "";
    for (const auto& seat : seats) {
        if (seat.getRow() != currentRow) {
            if (!currentRow.empty()) cout << endl;
            currentRow = seat.getRow();
            cout << currentRow << ": ";
        }
        
        if (seat.isAvailable()) {
            cout << "[A]";
        } else if (seat.isHeld()) {
            cout << "[H]";
        } else if (seat.isSold()) {
            cout << "[X]";
        }
        cout << " ";
    }
    cout << endl;
}

void BookingService::displayAvailableSeats(int showtimeId) const {
    if (showtimeSeats.find(showtimeId) == showtimeSeats.end()) {
        cout << "No seats available for showtime " << showtimeId << endl;
        return;
    }
    
    const vector<Seat>& seats = showtimeSeats.at(showtimeId);
    
    cout << "\n=== AVAILABLE SEATS ===" << endl;
    for (const auto& seat : seats) {
        if (seat.isAvailable()) {
            cout << seat.getSeatId() << "(" << seat.getType() << ") ";
        }
    }
    cout << endl;
}

void BookingService::displayAllOrders() const {
    cout << "\n=== ALL ORDERS ===" << endl;
    for (const auto& order : orders) {
        order.displayInfo();
        cout << "---" << endl;
    }
}

void BookingService::displayOrderHistory(int staffId) const {
    cout << "\n=== ORDER HISTORY FOR STAFF " << staffId << " ===" << endl;
    vector<Order> staffOrders = getOrdersByStaff(staffId);
    for (const auto& order : staffOrders) {
        order.displayInfo();
        cout << "---" << endl;
    }
}

string BookingService::formatTime(time_t timeValue) const {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", localtime(&timeValue));
    return string(buffer);
}

void BookingService::printTicketToFile(const Ticket& ticket, const string& filename) const {
    ofstream file(filename);
    if (file.is_open()) {
        file << "========== CINEMA TICKET ==========" << endl;
        file << "Ticket ID: " << ticket.getTicketId() << endl;
        file << "Movie: " << ticket.getMovieTitle() << endl;
        file << "Auditorium: " << ticket.getAuditoriumName() << endl;
        file << "Seat: " << ticket.getSeatId() << endl;
        file << "Show Time: " << formatTime(ticket.getShowTime()) << endl;
        file << "Price: $" << fixed << setprecision(2) << ticket.getPrice() << endl;
        file << "Issued: " << formatTime(ticket.getIssuedAt()) << endl;
        file << "===================================" << endl;
        file.close();
        cout << "Ticket printed to file: " << filename << endl;
    } else {
        cout << "Error: Could not create ticket file!" << endl;
    }
}

// Demo functions
void BookingService::selectMovieDemo() {
    cout << "\n=== SELECT MOVIE ===" << endl;
    cout << "Available movies:" << endl;
    cout << "1. Aquaman (ID: 1)" << endl;
    cout << "2. Spider-Man (ID: 2)" << endl;
    cout << "3. The Batman (ID: 3)" << endl;
    
    int movieId;
    cout << "Enter movie ID: ";
    cin >> movieId;
    
    cout << "Movie " << movieId << " selected!" << endl;
}

void BookingService::selectShowtimeDemo() {
    cout << "\n=== SELECT SHOWTIME ===" << endl;
    cout << "Available showtimes:" << endl;
    cout << "1. Today 14:00 - Theater 1 (ID: 1)" << endl;
    cout << "2. Today 17:00 - IMAX Theater (ID: 2)" << endl;
    cout << "3. Tomorrow 20:00 - 4DX Theater (ID: 3)" << endl;
    
    int showtimeId;
    cout << "Enter showtime ID: ";
    cin >> showtimeId;
    
    displaySeatMap(showtimeId);
    cout << "Showtime " << showtimeId << " selected!" << endl;
}

void BookingService::selectSeatsDemo() {
    cout << "\n=== SELECT SEATS ===" << endl;
    
    int showtimeId = 1; // Default showtime
    displaySeatMap(showtimeId);
    displayAvailableSeats(showtimeId);
    
    cout << "Enter seat IDs (comma-separated, e.g., A01,A02): ";
    string seatInput;
    cin.ignore();
    getline(cin, seatInput);
    
    // Parse seat IDs
    vector<string> seatIds;
    stringstream ss(seatInput);
    string seat;
    while (getline(ss, seat, ',')) {
        seatIds.push_back(seat);
    }
    
    if (holdSeats(showtimeId, seatIds)) {
        cout << "Seats selected and held!" << endl;
    }
}

void BookingService::calculatePriceDemo() {
    cout << "\n=== CALCULATE PRICE ===" << endl;
    
    vector<string> sampleSeats = {"A01", "A02"};
    double basePrice = 12.0;
    double total = calculateOrderTotal(1, sampleSeats, basePrice);
    
    cout << "Base price per seat: $" << basePrice << endl;
    cout << "Number of seats: " << sampleSeats.size() << endl;
    cout << "Subtotal: $" << (basePrice * sampleSeats.size()) << endl;
    cout << "Tax (10%): $" << (basePrice * sampleSeats.size() * 0.1) << endl;
    cout << "Total: $" << total << endl;
}

void BookingService::printTicketDemo() {
    cout << "\n=== PRINT TICKET ===" << endl;
    
    if (tickets.empty()) {
        cout << "No tickets available to print!" << endl;
        return;
    }
    
    const Ticket& ticket = tickets[0];
    ticket.displayTicket();
    
    cout << "Print to file? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        string filename = "ticket_" + ticket.getTicketId() + ".txt";
        printTicketToFile(ticket, filename);
    }
}

void BookingService::exchangeTicketDemo() {
    cout << "\n=== EXCHANGE TICKET ===" << endl;
    displayAllOrders();
    
    int orderId;
    cout << "Enter order ID to exchange: ";
    cin >> orderId;
    
    int newShowtimeId;
    cout << "Enter new showtime ID: ";
    cin >> newShowtimeId;
    
    vector<string> newSeats = {"B01", "B02"}; // Sample new seats
    
    if (exchangeTicket(orderId, newShowtimeId, newSeats)) {
        cout << "Ticket exchanged successfully!" << endl;
    }
}

void BookingService::refundTicketDemo() {
    cout << "\n=== REFUND TICKET ===" << endl;
    displayAllOrders();
    
    int orderId;
    cout << "Enter order ID to refund: ";
    cin >> orderId;
    
    string reason;
    cout << "Enter refund reason: ";
    cin.ignore();
    getline(cin, reason);
    
    if (refundTicket(orderId, reason)) {
        cout << "Ticket refunded successfully!" << endl;
    }
}
