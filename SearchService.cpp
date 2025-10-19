#include "SearchService.h"

void SearchService::searchAll(const std::string& query) const {
    std::cout << "\n--- Search Results for: " << query << " ---\n";

    // Search movies
    auto movies = movieService->searchMovies(query);
    if (!movies.empty()) {
        std::cout << "\nMovies:\n";
        for (const auto& movie : movies) movie.displayInfo();
    }

    // Search showtimes
    auto showtimes = showtimeService->searchShowtimes(query);
    if (!showtimes.empty()) {
        std::cout << "\nShowtimes:\n";
        for (const auto& show : showtimes) show.displayInfo();
    }

    // Search tickets
    // (Bạn có thể thêm hàm search ticket theo ID hoặc tên phim nếu muốn)

    // Search payments
    auto payments = paymentService->getPaymentsByMethod(query);
    if (!payments.empty()) {
        std::cout << "\nPayments:\n";
        for (const auto& pay : payments) pay->displayInfo();
    }
}

void SearchService::demonstrateSearch() const {
    std::cout << "Enter search keyword: ";
    std::string keyword;
    std::cin.ignore();
    std::getline(std::cin, keyword);
    searchAll(keyword);
}

void SearchService::lookupByTicketId() const {
    std::cout << "Enter ticket ID: ";
    std::string ticketId;
    std::cin >> ticketId;
    // Giả sử bạn có thể truy cập bookingService
    auto ticket = bookingService->findTicketById(ticketId);
    if (ticket) ticket->displayTicket();
    else std::cout << "Ticket not found!\n";
}

void SearchService::lookupByShowId() const {
    std::cout << "Enter showtime ID: ";
    int showId;
    std::cin >> showId;
    auto show = showtimeService->findShowtimeById(showId);
    if (show) show->displayInfo();
    else std::cout << "Showtime not found!\n";
}