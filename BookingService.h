#ifndef BOOKINGSERVICE_H
#define BOOKINGSERVICE_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <map>

using namespace std;

// Seat class
class Seat {
private:
    string seat_id;
    string row;
    int number;
    string type; // Standard, VIP, Couple, etc.
    string status; // available, held, sold
    time_t hold_expires_at;
    int order_id;
    double price_multiplier;

public:
    Seat();
    Seat(const string& seatId, const string& seatType = "Standard");
    
    // Getters
    string getSeatId() const { return seat_id; }
    string getRow() const { return row; }
    int getNumber() const { return number; }
    string getType() const { return type; }
    string getStatus() const { return status; }
    time_t getHoldExpiresAt() const { return hold_expires_at; }
    int getOrderId() const { return order_id; }
    double getPriceMultiplier() const { return price_multiplier; }
    
    // Setters
    void setSeatId(const string& newSeatId);
    void setType(const string& newType) { type = newType; }
    void setStatus(const string& newStatus) { status = newStatus; }
    void setHoldExpiresAt(time_t newExpiry) { hold_expires_at = newExpiry; }
    void setOrderId(int newOrderId) { order_id = newOrderId; }
    void setPriceMultiplier(double newMultiplier) { price_multiplier = newMultiplier; }
    
    bool isAvailable() const;
    bool isHeld() const;
    bool isSold() const;
    bool isHoldExpired() const;
    void displayInfo() const;
};

// Order class
class Order {
private:
    int id;
    int staff_id;
    int showtime_id;
    vector<string> seat_ids;
    double subtotal;
    double tax;
    double discount;
    double total_amount;
    string payment_status; // pending, paid, refunded
    string customer_name;
    string customer_phone;
    time_t created_at;
    time_t updated_at;

public:
    Order();
    Order(int staffId, int showtimeId, const vector<string>& seatIds);
    
    // Getters
    int getId() const { return id; }
    int getStaffId() const { return staff_id; }
    int getShowtimeId() const { return showtime_id; }
    vector<string> getSeatIds() const { return seat_ids; }
    double getSubtotal() const { return subtotal; }
    double getTax() const { return tax; }
    double getDiscount() const { return discount; }
    double getTotalAmount() const { return total_amount; }
    string getPaymentStatus() const { return payment_status; }
    string getCustomerName() const { return customer_name; }
    string getCustomerPhone() const { return customer_phone; }
    time_t getCreatedAt() const { return created_at; }
    time_t getUpdatedAt() const { return updated_at; }
    
    // Setters
    void setId(int newId) { id = newId; }
    void setStaffId(int newStaffId) { staff_id = newStaffId; }
    void setShowtimeId(int newShowtimeId) { showtime_id = newShowtimeId; }
    void setSeatIds(const vector<string>& newSeatIds) { seat_ids = newSeatIds; }
    void setSubtotal(double newSubtotal) { subtotal = newSubtotal; }
    void setTax(double newTax) { tax = newTax; }
    void setDiscount(double newDiscount) { discount = newDiscount; }
    void setTotalAmount(double newTotal) { total_amount = newTotal; }
    void setPaymentStatus(const string& newStatus) { payment_status = newStatus; updated_at = time(0); }
    void setCustomerName(const string& newName) { customer_name = newName; }
    void setCustomerPhone(const string& newPhone) { customer_phone = newPhone; }
    
    void calculateTotal();
    void displayInfo() const;
    bool isValid() const;
};

// Ticket class
class Ticket {
private:
    string ticket_id;
    int order_id;
    int showtime_id;
    string seat_id;
    string movie_title;
    string auditorium_name;
    time_t show_time;
    double price;
    string status; // valid, used, canceled
    time_t issued_at;

public:
    Ticket();
    Ticket(int orderId, int showtimeId, const string& seatId);
    
    // Getters
    string getTicketId() const { return ticket_id; }
    int getOrderId() const { return order_id; }
    int getShowtimeId() const { return showtime_id; }
    string getSeatId() const { return seat_id; }
    string getMovieTitle() const { return movie_title; }
    string getAuditoriumName() const { return auditorium_name; }
    time_t getShowTime() const { return show_time; }
    double getPrice() const { return price; }
    string getStatus() const { return status; }
    time_t getIssuedAt() const { return issued_at; }
    
    // Setters
    void setTicketId(const string& newTicketId) { ticket_id = newTicketId; }
    void setOrderId(int newOrderId) { order_id = newOrderId; }
    void setShowtimeId(int newShowtimeId) { showtime_id = newShowtimeId; }
    void setSeatId(const string& newSeatId) { seat_id = newSeatId; }
    void setMovieTitle(const string& newTitle) { movie_title = newTitle; }
    void setAuditoriumName(const string& newName) { auditorium_name = newName; }
    void setShowTime(time_t newShowTime) { show_time = newShowTime; }
    void setPrice(double newPrice) { price = newPrice; }
    void setStatus(const string& newStatus) { status = newStatus; }
    
    void displayTicket() const;
    string generateTicketId() const;
    bool isValid() const;
};

// BookingService class - Business Logic Layer
class BookingService {
private:
    vector<Order> orders;
    vector<Ticket> tickets;
    map<int, vector<Seat>> showtimeSeats; // showtime_id -> seats
    int nextOrderId;
    int nextTicketId;
    
    bool validateSeatSelection(int showtimeId, const vector<string>& seatIds) const;
    double calculateSeatPrice(const string& seatId, double basePrice) const;
    void initializeSeatsForShowtime(int showtimeId, int totalSeats);
    vector<Seat> heapSortSeats(vector<Seat> seatList, bool byPrice = false) const;

public:
    BookingService();
    
    // Seat management
    vector<Seat> getSeatsForShowtime(int showtimeId);
    bool holdSeats(int showtimeId, const vector<string>& seatIds, int holdTimeMinutes = 5);
    bool releaseHeldSeats(int showtimeId, const vector<string>& seatIds);
    void releaseExpiredHolds();
    bool areSeatsAvailable(int showtimeId, const vector<string>& seatIds) const;
    
    // Order management
    bool createOrder(const Order& order);
    bool updateOrder(int orderId, const Order& updatedOrder);
    Order* findOrderById(int orderId);
    vector<Order> getOrdersByStaff(int staffId) const;
    vector<Order> getOrdersByShowtime(int showtimeId) const;
    
    // Booking process
    bool confirmBooking(int orderId);
    bool cancelBooking(int orderId, const string& reason = "");
    bool exchangeTicket(int orderId, int newShowtimeId, const vector<string>& newSeatIds);
    bool refundTicket(int orderId, const string& reason = "");
    
    // Ticket management
    bool issueTickets(int orderId);
    vector<Ticket> getTicketsByOrder(int orderId) const;
    Ticket* findTicketById(const string& ticketId);
    bool validateTicket(const string& ticketId) const;
    
    // Pricing
    double calculateOrderTotal(int showtimeId, const vector<string>& seatIds, 
                              double basePrice, double taxRate = 0.1, double discount = 0.0) const;
    
    // Seat map display
    void displaySeatMap(int showtimeId) const;
    void displayAvailableSeats(int showtimeId) const;
    
    // Demo functions for terminal UI
    void selectMovieDemo();
    void selectShowtimeDemo();
    void selectSeatsDemo();
    void calculatePriceDemo();
    void printTicketDemo();
    void exchangeTicketDemo();
    void refundTicketDemo();
    
    // Utility
    void displayAllOrders() const;
    void displayOrderHistory(int staffId) const;
    string formatTime(time_t timeValue) const;
    void printTicketToFile(const Ticket& ticket, const string& filename) const;
};

#endif
