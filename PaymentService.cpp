#include "PaymentService.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>

// Payment base class implementation
Payment::Payment() : id(0), order_id(0), amount(0.0), status("pending"),
                     created_at(time(0)), updated_at(time(0)) {}

Payment::Payment(int orderId, double paymentAmount) 
    : id(0), order_id(orderId), amount(paymentAmount), status("pending"),
      created_at(time(0)), updated_at(time(0)) {}

void Payment::displayInfo() const {
    cout << "Payment ID: " << id << " | Order: " << order_id 
         << " | Amount: $" << fixed << setprecision(2) << amount
         << " | Method: " << getPaymentMethod() << " | Status: " << status << endl;
    
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&created_at));
    cout << "Created: " << timeBuffer;
    
    if (updated_at != created_at) {
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&updated_at));
        cout << " | Updated: " << timeBuffer;
    }
    cout << endl;
}

bool Payment::isValid() const {
    return order_id > 0 && amount > 0;
}

// CashPayment class implementation
CashPayment::CashPayment() : Payment(), cash_received(0.0), change_given(0.0), cashier_id(0) {}

CashPayment::CashPayment(int orderId, double paymentAmount, double cashReceived, int cashierId)
    : Payment(orderId, paymentAmount), cash_received(cashReceived), cashier_id(cashierId) {
    setCashReceived(cashReceived);
}

void CashPayment::setCashReceived(double newCashReceived) {
    cash_received = newCashReceived;
    if (cash_received >= amount) {
        change_given = cash_received - amount;
    } else {
        change_given = 0.0;
    }
}

bool CashPayment::processPayment() {
    if (!isValid()) {
        cout << "Error: Invalid cash payment data!" << endl;
        return false;
    }
    
    if (cash_received < amount) {
        cout << "Error: Insufficient cash received! Need $" << amount 
             << ", received $" << cash_received << endl;
        return false;
    }
    
    setStatus("completed");
    cout << "Cash payment processed successfully!" << endl;
    cout << "Change to give: $" << fixed << setprecision(2) << change_given << endl;
    
    return true;
}

bool CashPayment::refundPayment() {
    if (status != "completed") {
        cout << "Error: Cannot refund non-completed payment!" << endl;
        return false;
    }
    
    setStatus("refunded");
    cout << "Cash refund of $" << fixed << setprecision(2) << amount 
         << " processed successfully!" << endl;
    
    return true;
}

void CashPayment::displayInfo() const {
    Payment::displayInfo();
    cout << "Cash Received: $" << fixed << setprecision(2) << cash_received
         << " | Change Given: $" << change_given 
         << " | Cashier ID: " << cashier_id << endl;
}

bool CashPayment::isValid() const {
    return Payment::isValid() && cashier_id > 0 && cash_received >= 0;
}

// WalletPayment class implementation
WalletPayment::WalletPayment() : Payment(), verification_required(true), verification_timeout(0) {}

WalletPayment::WalletPayment(int orderId, double paymentAmount, const string& walletType)
    : Payment(orderId, paymentAmount), wallet_type(walletType), 
      verification_required(true), verification_timeout(time(0) + 300) { // 5 minutes timeout
    qr_code = generateQrCode();
}

bool WalletPayment::processPayment() {
    if (!isValid()) {
        cout << "Error: Invalid wallet payment data!" << endl;
        return false;
    }
    
    cout << "Processing " << wallet_type << " payment..." << endl;
    cout << "QR Code: " << qr_code << endl;
    cout << "Please scan QR code or complete payment in your wallet app." << endl;
    
    // Simulate payment verification
    if (verification_required) {
        cout << "Waiting for payment verification..." << endl;
        // In real implementation, this would check with payment gateway
        if (verifyPayment()) {
            setStatus("completed");
            transaction_id = "TXN" + to_string(time(0));
            cout << "Wallet payment completed! Transaction ID: " << transaction_id << endl;
            return true;
        } else {
            setStatus("failed");
            cout << "Payment verification failed or timed out!" << endl;
            return false;
        }
    }
    
    return false;
}

bool WalletPayment::refundPayment() {
    if (status != "completed") {
        cout << "Error: Cannot refund non-completed payment!" << endl;
        return false;
    }
    
    setStatus("refunded");
    cout << "Wallet refund of $" << fixed << setprecision(2) << amount 
         << " initiated. Transaction ID: " << transaction_id << endl;
    
    return true;
}

void WalletPayment::displayInfo() const {
    Payment::displayInfo();
    cout << "Wallet Type: " << wallet_type;
    if (!transaction_id.empty()) {
        cout << " | Transaction ID: " << transaction_id;
    }
    if (!qr_code.empty()) {
        cout << " | QR Code: " << qr_code;
    }
    cout << endl;
}

bool WalletPayment::isValid() const {
    return Payment::isValid() && !wallet_type.empty();
}

bool WalletPayment::verifyPayment() {
    // Simulate payment verification process
    if (isVerificationExpired()) {
        cout << "Payment verification expired!" << endl;
        return false;
    }
    
    // In real implementation, this would call payment gateway API
    cout << "Verifying payment with " << wallet_type << " gateway..." << endl;
    
    // Simulate random success/failure for demo
    bool success = (rand() % 10) > 2; // 80% success rate
    
    if (success) {
        verification_required = false;
        return true;
    }
    
    return false;
}

bool WalletPayment::isVerificationExpired() const {
    return verification_required && time(0) > verification_timeout;
}

string WalletPayment::generateQrCode() const {
    stringstream ss;
    ss << wallet_type << "_QR_" << order_id << "_" << time(0);
    return ss.str();
}

// CardPayment class implementation
CardPayment::CardPayment() : Payment(), is_contactless(false) {}

CardPayment::CardPayment(int orderId, double paymentAmount, const string& cardType)
    : Payment(orderId, paymentAmount), card_type(cardType), is_contactless(false) {}

bool CardPayment::processPayment() {
    if (!isValid()) {
        cout << "Error: Invalid card payment data!" << endl;
        return false;
    }
    
    cout << "Processing " << card_type << " card payment..." << endl;
    
    if (is_contactless) {
        cout << "Please tap your card on the reader." << endl;
    } else {
        cout << "Please insert your card and enter PIN." << endl;
    }
    
    // Simulate card processing
    cout << "Contacting card issuer..." << endl;
    
    // Simulate random success/failure for demo
    bool success = (rand() % 10) > 1; // 90% success rate
    
    if (success) {
        setStatus("completed");
        authorization_code = "AUTH" + to_string(time(0));
        cout << "Card payment approved! Authorization: " << authorization_code << endl;
        return true;
    } else {
        setStatus("failed");
        cout << "Card payment declined!" << endl;
        return false;
    }
}

bool CardPayment::refundPayment() {
    if (status != "completed") {
        cout << "Error: Cannot refund non-completed payment!" << endl;
        return false;
    }
    
    setStatus("refunded");
    cout << "Card refund of $" << fixed << setprecision(2) << amount 
         << " processed. Authorization: " << authorization_code << endl;
    
    return true;
}

void CardPayment::displayInfo() const {
    Payment::displayInfo();
    cout << "Card Type: " << card_type;
    if (!card_number_masked.empty()) {
        cout << " | Card: " << card_number_masked;
    }
    if (!authorization_code.empty()) {
        cout << " | Auth: " << authorization_code;
    }
    if (is_contactless) {
        cout << " | Contactless";
    }
    cout << endl;
}

bool CardPayment::isValid() const {
    return Payment::isValid() && !card_type.empty();
}

// PaymentService class implementation
PaymentService::PaymentService() : nextPaymentId(1) {
    // Initialize with some sample data
    srand(time(0)); // For random simulation
}

PaymentService::~PaymentService() {
    // Clean up dynamically allocated payments
    for (Payment* payment : payments) {
        delete payment;
    }
    payments.clear();
}

bool PaymentService::validatePaymentAmount(double amount) const {
    if (amount <= 0) {
        cout << "Error: Payment amount must be positive!" << endl;
        return false;
    }
    if (amount > 10000) { // Arbitrary large amount check
        cout << "Error: Payment amount exceeds maximum limit!" << endl;
        return false;
    }
    return true;
}

string PaymentService::getCurrentDate() const {
    time_t now = time(0);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&now));
    return string(buffer);
}

// Heap sort implementation for payments
vector<Payment*> PaymentService::heapSortPayments(vector<Payment*> paymentList, bool byAmount) const {
    make_heap(paymentList.begin(), paymentList.end(), [byAmount](const Payment* a, const Payment* b) {
        if (byAmount) {
            return a->getAmount() < b->getAmount(); // Sort by amount (highest first)
        }
        return a->getCreatedAt() > b->getCreatedAt(); // Sort by time (newest first)
    });
    
    sort_heap(paymentList.begin(), paymentList.end(), [byAmount](const Payment* a, const Payment* b) {
        if (byAmount) {
            return a->getAmount() < b->getAmount();
        }
        return a->getCreatedAt() > b->getCreatedAt();
    });
    
    return paymentList;
}

bool PaymentService::processPayment(Payment* payment) {
    if (!payment) {
        cout << "Error: Null payment object!" << endl;
        return false;
    }
    
    if (!validatePaymentAmount(payment->getAmount())) {
        return false;
    }
    
    payment->setId(nextPaymentId++);
    
    if (payment->processPayment()) {
        payments.push_back(payment);
        
        // Update daily totals
        string date = getCurrentDate();
        dailyTotals[date] += payment->getAmount();
        
        cout << "Payment processed and recorded successfully!" << endl;
        return true;
    } else {
        cout << "Payment processing failed!" << endl;
        return false;
    }
}

bool PaymentService::refundPayment(int paymentId, const string& reason) {
    Payment* payment = findPaymentById(paymentId);
    if (!payment) {
        cout << "Error: Payment with ID " << paymentId << " not found!" << endl;
        return false;
    }
    
    if (payment->refundPayment()) {
        // Update daily totals (subtract refunded amount)
        string date = getCurrentDate();
        dailyTotals[date] -= payment->getAmount();
        
        cout << "Refund processed successfully. Reason: " << reason << endl;
        return true;
    }
    
    return false;
}

bool PaymentService::voidPayment(int paymentId) {
    Payment* payment = findPaymentById(paymentId);
    if (!payment) {
        cout << "Error: Payment with ID " << paymentId << " not found!" << endl;
        return false;
    }
    
    if (payment->getStatus() == "pending") {
        payment->setStatus("voided");
        cout << "Payment voided successfully!" << endl;
        return true;
    } else {
        cout << "Error: Can only void pending payments!" << endl;
        return false;
    }
}

Payment* PaymentService::createCashPayment(int orderId, double amount, double cashReceived, int cashierId) {
    return new CashPayment(orderId, amount, cashReceived, cashierId);
}

Payment* PaymentService::createWalletPayment(int orderId, double amount, const string& walletType) {
    return new WalletPayment(orderId, amount, walletType);
}

Payment* PaymentService::createCardPayment(int orderId, double amount, const string& cardType) {
    return new CardPayment(orderId, amount, cardType);
}

Payment* PaymentService::findPaymentById(int paymentId) {
    for (Payment* payment : payments) {
        if (payment->getId() == paymentId) {
            return payment;
        }
    }
    return nullptr;
}

vector<Payment*> PaymentService::getPaymentsByOrder(int orderId) const {
    vector<Payment*> results;
    for (Payment* payment : payments) {
        if (payment->getOrderId() == orderId) {
            results.push_back(payment);
        }
    }
    return results;
}

vector<Payment*> PaymentService::getPaymentsByMethod(const string& method) const {
    vector<Payment*> results;
    for (Payment* payment : payments) {
        if (payment->getPaymentMethod().find(method) != string::npos) {
            results.push_back(payment);
        }
    }
    return results;
}

vector<Payment*> PaymentService::getPaymentsByStatus(const string& status) const {
    vector<Payment*> results;
    for (Payment* payment : payments) {
        if (payment->getStatus() == status) {
            results.push_back(payment);
        }
    }
    return results;
}

vector<Payment*> PaymentService::getPaymentsByDate(const string& date) const {
    vector<Payment*> results;
    for (Payment* payment : payments) {
    char dateBuffer[11];
    time_t createdAt = payment->getCreatedAt(); // Tạo biến tạm
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", localtime(&createdAt));
    if (string(dateBuffer) == date) {
        results.push_back(payment);
    }
}
    return results;
}

double PaymentService::getDailyTotal(const string& date) const {
    auto it = dailyTotals.find(date);
    return (it != dailyTotals.end()) ? it->second : 0.0;
}

double PaymentService::getWeeklyTotal() const {
    double total = 0.0;
    time_t now = time(0);
    time_t weekAgo = now - 7 * 24 * 3600;
    
    for (Payment* payment : payments) {
        if (payment->getCreatedAt() >= weekAgo && payment->getStatus() == "completed") {
            total += payment->getAmount();
        }
    }
    
    return total;
}

double PaymentService::getMonthlyTotal() const {
    double total = 0.0;
    time_t now = time(0);
    time_t monthAgo = now - 30 * 24 * 3600;
    
    for (Payment* payment : payments) {
        if (payment->getCreatedAt() >= monthAgo && payment->getStatus() == "completed") {
            total += payment->getAmount();
        }
    }
    
    return total;
}

map<string, double> PaymentService::getPaymentMethodTotals() const {
    map<string, double> totals;
    
    for (Payment* payment : payments) {
        if (payment->getStatus() == "completed") {
            string method = payment->getPaymentMethod();
            totals[method] += payment->getAmount();
        }
    }
    
    return totals;
}

vector<Payment*> PaymentService::getTopPayments(int limit) const {
    vector<Payment*> completedPayments = getPaymentsByStatus("completed");
    vector<Payment*> sortedPayments = heapSortPayments(completedPayments, true); // Sort by amount
    
    if (sortedPayments.size() > static_cast<size_t>(limit)) {
        sortedPayments.resize(limit);
    }
    
    return sortedPayments;
}

bool PaymentService::reconcileTransactions(const string& date) {
    cout << "\n=== RECONCILING TRANSACTIONS FOR " << date << " ===" << endl;
    
    vector<Payment*> dayPayments = getPaymentsByDate(date);
    double totalProcessed = 0.0;
    double totalCompleted = 0.0;
    int completedCount = 0;
    int failedCount = 0;
    int pendingCount = 0;
    
    for (Payment* payment : dayPayments) {
        totalProcessed += payment->getAmount();
        
        if (payment->getStatus() == "completed") {
            totalCompleted += payment->getAmount();
            completedCount++;
        } else if (payment->getStatus() == "failed") {
            failedCount++;
        } else if (payment->getStatus() == "pending") {
            pendingCount++;
        }
    }
    
    cout << "Total transactions: " << dayPayments.size() << endl;
    cout << "Completed: " << completedCount << " ($" << fixed << setprecision(2) << totalCompleted << ")" << endl;
    cout << "Failed: " << failedCount << endl;
    cout << "Pending: " << pendingCount << endl;
    cout << "Daily total recorded: $" << getDailyTotal(date) << endl;
    
    bool reconciled = (abs(totalCompleted - getDailyTotal(date)) < 0.01); // Allow for floating point precision
    
    if (reconciled) {
        cout << "✓ Reconciliation successful!" << endl;
    } else {
        cout << "✗ Reconciliation failed - totals do not match!" << endl;
    }
    
    return reconciled;
}

vector<Payment*> PaymentService::getPendingPayments() const {
    return getPaymentsByStatus("pending");
}

vector<Payment*> PaymentService::getFailedPayments() const {
    return getPaymentsByStatus("failed");
}
void PaymentService::processPaymentDemo() {
    cout << "[Demo] Process Payment Demo" << endl;
}

void PaymentService::refundPaymentDemo() {
    cout << "[Demo] Refund Payment Demo" << endl;
}

void PaymentService::viewPaymentHistoryDemo() {
    cout << "[Demo] View Payment History Demo" << endl;
}

void PaymentService::reconcileDemo() {
    cout << "[Demo] Reconcile Demo" << endl;
}

void PaymentService::paymentReportsDemo() {
    cout << "[Demo] Payment Reports Demo" << endl;
}

// Utility
void PaymentService::displayAllPayments() const {
    for (Payment* payment : payments) {
        payment->displayInfo();
        cout << endl;
    }
}

void PaymentService::displayPaymentSummary() const {
    cout << "Total Payments: " << payments.size() << endl;
}

string PaymentService::formatTime(time_t timeValue) const {
    char timeBuffer[80];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M", localtime(&timeValue));
    return string(timeBuffer);
}

string PaymentService::maskCardNumber(const string& cardNumber) const {
    if (cardNumber.empty()) return "";
    if (cardNumber.length() < 4) return string(cardNumber.length(), '*');
    return string(cardNumber.length() - 4, '*') + cardNumber.substr(cardNumber.length() - 4);
}