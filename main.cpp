#include <iostream>
#include <string>
#include <vector>
#include "MovieService.h"
#include "ShowtimeService.h"
#include "BookingService.h"
#include "PaymentService.h"
#include "SearchService.h"

using namespace std;

class CinemaSystem {
private:
    MovieService movieService;
    ShowtimeService showtimeService;
    BookingService bookingService;
    PaymentService paymentService;
    SearchService searchService;
    
public:
    void displayMainMenu() {
        cout << "\n=== CINEMA BOOKING SYSTEM ===" << endl;
        cout << "1. Movie Management" << endl;
        cout << "2. Showtime Management" << endl;
        cout << "3. Ticket Booking & Payment" << endl;
        cout << "4. Search Functions" << endl;
        cout << "5. Quick Lookup" << endl;
        cout << "0. Exit" << endl;
        cout << "Choose option: ";
    }
    
    void displayMovieMenu() {
        cout << "\n=== MOVIE MANAGEMENT ===" << endl;
        cout << "1. Add Movie" << endl;
        cout << "2. Update Movie" << endl;
        cout << "3. Search Movies" << endl;
        cout << "4. Archive Movie" << endl;
        cout << "5. Bulk Import Movies" << endl;
        cout << "6. Movie Statistics" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Choose option: ";
    }
    
    void displayShowtimeMenu() {
        cout << "\n=== SHOWTIME MANAGEMENT ===" << endl;
        cout << "1. Create Showtime" << endl;
        cout << "2. Update Showtime" << endl;
        cout << "3. Cancel Showtime" << endl;
        cout << "4. Search Showtimes" << endl;
        cout << "5. Bulk Create Showtimes" << endl;
        cout << "6. Copy Schedule" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Choose option: ";
    }
    
    void displayBookingMenu() {
        cout << "\n=== BOOKING & PAYMENT ===" << endl;
        cout << "1. Select Movie" << endl;
        cout << "2. Select Showtime" << endl;
        cout << "3. Select Seats" << endl;
        cout << "4. Calculate Price" << endl;
        cout << "5. Process Payment" << endl;
        cout << "6. Print Ticket" << endl;
        cout << "7. Exchange Ticket" << endl;
        cout << "8. Refund Ticket" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Choose option: ";
    }
    
    void run() {
        int choice;
        do {
            displayMainMenu();
            cin >> choice;
            
            switch(choice) {
                case 1:
                    handleMovieManagement();
                    break;
                case 2:
                    handleShowtimeManagement();
                    break;
                case 3:
                    handleBookingManagement();
                    break;
                case 4:
                    handleSearchFunctions();
                    break;
                case 5:
                    handleQuickLookup();
                    break;
                case 0:
                    cout << "Goodbye!" << endl;
                    break;
                default:
                    cout << "Invalid option!" << endl;
            }
        } while(choice != 0);
    }
    
private:
    void handleMovieManagement() {
        int choice;
        do {
            displayMovieMenu();
            cin >> choice;
            
            switch(choice) {
                case 1:
                    movieService.createMovieDemo();
                    break;
                case 2:
                    movieService.updateMovieDemo();
                    break;
                case 3:
                    movieService.searchMoviesDemo();
                    break;
                case 4:
                    movieService.archiveMovieDemo();
                    break;
                case 5:
                    movieService.bulkImportDemo();
                    break;
                case 6:
                    movieService.showStatisticsDemo();
                    break;
                case 0:
                    return;
                default:
                    cout << "Invalid option!" << endl;
            }
        } while(choice != 0);
    }
    
    void handleShowtimeManagement() {
        int choice;
        do {
            displayShowtimeMenu();
            cin >> choice;
            
            switch(choice) {
                case 1:
                    showtimeService.createShowtimeDemo();
                    break;
                case 2:
                    showtimeService.updateShowtimeDemo();
                    break;
                case 3:
                    showtimeService.cancelShowtimeDemo();
                    break;
                case 4:
                    showtimeService.searchShowtimesDemo();
                    break;
                case 0:
                    return;
                default:
                    cout << "Invalid option!" << endl;
            }
        } while(choice != 0);
    }
    
    void handleBookingManagement() {
        int choice;
        do {
            displayBookingMenu();
            cin >> choice;
            
            switch(choice) {
                case 1:
                    bookingService.selectMovieDemo();
                    break;
                case 2:
                    bookingService.selectShowtimeDemo();
                    break;
                case 3:
                    bookingService.selectSeatsDemo();
                    break;
                case 4:
                    bookingService.calculatePriceDemo();
                    break;
                case 5:
                    paymentService.processPaymentDemo();
                    break;
                case 6:
                    bookingService.printTicketDemo();
                    break;
                case 0:
                    return;
                default:
                    cout << "Invalid option!" << endl;
            }
        } while(choice != 0);
    }
    
    void handleSearchFunctions() {
        cout << "\n=== SEARCH FUNCTIONS ===" << endl;
        searchService.demonstrateSearch();
    }
    
    void handleQuickLookup() {
        cout << "\n=== QUICK LOOKUP ===" << endl;
        cout << "1. Lookup by Ticket ID" << endl;
        cout << "2. Lookup by Show ID" << endl;
        int choice;
        cin >> choice;
        
        if(choice == 1) {
            searchService.lookupByTicketId();
        } else if(choice == 2) {
            searchService.lookupByShowId();
        }
    }
};

int main() {
    CinemaSystem system;
    system.run();
    return 0;
}
