#ifndef SEARCHSERVICE_H
#define SEARCHSERVICE_H

#include <iostream>
#include <string>
#include "MovieService.h"
#include "ShowtimeService.h"
#include "BookingService.h"
#include "PaymentService.h"

class SearchService {
private:
    MovieService* movieService;
    ShowtimeService* showtimeService;
    BookingService* bookingService;
    PaymentService* paymentService;

public:
    SearchService(MovieService* m, ShowtimeService* s, BookingService* b, PaymentService* p)
        : movieService(m), showtimeService(s), bookingService(b), paymentService(p) {}

    void searchAll(const std::string& query) const;
    void demonstrateSearch() const;
    void lookupByTicketId() const;
    void lookupByShowId() const;
};

#endif