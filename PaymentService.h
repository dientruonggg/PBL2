#ifndef PAYMENTSERVICE_H
#define PAYMENTSERVICE_H

#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <map>

using namespace std;

// Payment base class
class Payment {
protected:
    int id;
    int order_id;
    double amount;
    string status; // pending, completed, failed, refunded
    string gateway_ref;
    time_t created_at;
    time_t updated_at;

public:
    Payment();
    Payment(int orderId, double paymentAmount);
    virtual ~Payment() {}
    
    // Getters
    int getId() const { return id; }
    int getOrderId() const { return order_id; }
    double getAmount() const { return amount; }
    string getStatus() const { return status; }
    string getGatewayRef() const { return gateway_ref; }
    time_t getCreatedAt() const { return created_at; }
    time_t getUpdatedAt() const { return updated_at; }
    
    // Setters
    void setId(int newId) { id = newId; }
    void setOrderId(int newOrderId) { order_id = newOrderId; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setStatus(const string& newStatus) { status = newStatus; updated_at = time(0); }
    void setGatewayRef(const string& newRef) { gateway_ref = newRef; }
    
    // Virtual methods
    virtual bool processPayment() = 0;
    virtual bool refundPayment() = 0;
    virtual string getPaymentMethod() const = 0;
    virtual void displayInfo() const;
    virtual bool isValid() const;
};

// Cash Payment class
class CashPayment : public Payment {
private:
    double cash_received;
    double change_given;
    int cashier_id;

public:
    CashPayment();
    CashPayment(int orderId, double paymentAmount, double cashReceived, int cashierId);
    
    // Getters
    double getCashReceived() const { return cash_received; }
    double getChangeGiven() const { return change_given; }
    int getCashierId() const { return cashier_id; }
    
    // Setters
    void setCashReceived(double newCashReceived);
    void setChangeGiven(double newChangeGiven) { change_given = newChangeGiven; }
    void setCashierId(int newCashierId) { cashier_id = newCashierId; }
    
    // Override virtual methods
    bool processPayment() override;
    bool refundPayment() override;
    string getPaymentMethod() const override { return "Cash"; }
    void displayInfo() const override;
    bool isValid() const override;
};

// QR/Digital Wallet Payment class
class WalletPayment : public Payment {
private:
    string wallet_type; // QR, PayPal, etc.
    string transaction_id;
    string qr_code;
    bool verification_required;
    time_t verification_timeout;

public:
    WalletPayment();
    WalletPayment(int orderId, double paymentAmount, const string& walletType);
    
    // Getters
    string getWalletType() const { return wallet_type; }
    string getTransactionId() const { return transaction_id; }
    string getQrCode() const { return qr_code; }
    bool isVerificationRequired() const { return verification_required; }
    time_t getVerificationTimeout() const { return verification_timeout; }
    
    // Setters
    void setWalletType(const string& newWalletType) { wallet_type = newWalletType; }
    void setTransactionId(const string& newTransactionId) { transaction_id = newTransactionId; }
    void setQrCode(const string& newQrCode) { qr_code = newQrCode; }
    void setVerificationRequired(bool required) { verification_required = required; }
    void setVerificationTimeout(time_t timeout) { verification_timeout = timeout; }
    
    // Override virtual methods
    bool processPayment() override;
    bool refundPayment() override;
    string getPaymentMethod() const override { return "Digital Wallet (" + wallet_type + ")"; }
    void displayInfo() const override;
    bool isValid() const override;
    
    // Wallet-specific methods
    bool verifyPayment();
    bool isVerificationExpired() const;
    string generateQrCode() const;
};

// Card Payment class
class CardPayment : public Payment {
private:
    string card_number_masked;
    string card_type; // Visa, MasterCard, etc.
    string authorization_code;
    bool is_contactless;

public:
    CardPayment();
    CardPayment(int orderId, double paymentAmount, const string& cardType);
    
    // Getters
    string getCardNumberMasked() const { return card_number_masked; }
    string getCardType() const { return card_type; }
    string getAuthorizationCode() const { return authorization_code; }
    bool isContactless() const { return is_contactless; }
    
    // Setters
    void setCardNumberMasked(const string& maskedNumber) { card_number_masked = maskedNumber; }
    void setCardType(const string& newCardType) { card_type = newCardType; }
    void setAuthorizationCode(const string& authCode) { authorization_code = authCode; }
    void setContactless(bool contactless) { is_contactless = contactless; }
    
    // Override virtual methods
    bool processPayment() override;
    bool refundPayment() override;
    string getPaymentMethod() const override { return "Card (" + card_type + ")"; }
    void displayInfo() const override;
    bool isValid() const override;
};

// PaymentService class - Business Logic Layer
class PaymentService {
private:
    vector<Payment*> payments;
    map<string, double> dailyTotals; // date -> total amount
    int nextPaymentId;
    
    bool validatePaymentAmount(double amount) const;
    string getCurrentDate() const;
    vector<Payment*> heapSortPayments(vector<Payment*> paymentList, bool byAmount = false) const;

public:
    PaymentService();
    ~PaymentService();
    
    // Payment processing
    bool processPayment(Payment* payment);
    bool refundPayment(int paymentId, const string& reason = "");
    bool voidPayment(int paymentId);
    
    // Payment creation
    Payment* createCashPayment(int orderId, double amount, double cashReceived, int cashierId);
    Payment* createWalletPayment(int orderId, double amount, const string& walletType);
    Payment* createCardPayment(int orderId, double amount, const string& cardType);
    
    // Payment retrieval
    Payment* findPaymentById(int paymentId);
    vector<Payment*> getPaymentsByOrder(int orderId) const;
    vector<Payment*> getPaymentsByMethod(const string& method) const;
    vector<Payment*> getPaymentsByStatus(const string& status) const;
    vector<Payment*> getPaymentsByDate(const string& date) const;
    
    // Financial reporting
    double getDailyTotal(const string& date) const;
    double getWeeklyTotal() const;
    double getMonthlyTotal() const;
    map<string, double> getPaymentMethodTotals() const;
    vector<Payment*> getTopPayments(int limit = 10) const;
    
    // Reconciliation
    bool reconcileTransactions(const string& date);
    vector<Payment*> getPendingPayments() const;
    vector<Payment*> getFailedPayments() const;
    
    // Validation and verification
    bool verifyWalletPayment(int paymentId);
    bool checkPaymentStatus(int paymentId);
    
    // Demo functions for terminal UI
    void processPaymentDemo();
    void refundPaymentDemo();
    void viewPaymentHistoryDemo();
    void reconcileDemo();
    void paymentReportsDemo();
    
    // Utility
    void displayAllPayments() const;
    void displayPaymentSummary() const;
    string formatTime(time_t timeValue) const;
    string maskCardNumber(const string& cardNumber) const;
};

#endif
