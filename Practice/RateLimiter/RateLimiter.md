Design and implement a thread-safe rate limiter in C++17.
The rate limiter controls how many requests a client is allowed to make within a fixed time window.

Functional Requirements - 
1. Per-client rate limiting
    Each request is identified by a clientId (e.g., int or std::string)
    Rate limits apply independently per client
2. Rate limit policy (fixed window)
    Allow at most N requests per T seconds per client
    Fixed Window algorithm
    Lazy window reset (no background thread)
3. API
    bool allowRequest(const ClientId& clientId);
    Returns true if the request is allowed
    Returns false if the request is rejected due to rate limiting
4. Time
    Use std::chrono::steady_clock
    Use duration arithmetic correctly

Concurrency Requirements
1. Thread-safe
    Multiple threads may call allowRequest() concurrently
2. Locking strategy
    One mutex protecting the client state map
    One mutex per client state
    Lock order:
        Map mutex (short duration)
        Client mutex (rate logic)
    No long-held global locks
    No nested locking beyond this order
3. Shared mutable state
    Per client:
        windowStartTime
        requestsCounter
    Protected by per-client mutex
4. Configuration
    maxRequestsPerWindow
    windowSize
    Stored in RateLimiter
    Immutable after construction
    No locking required for access

