#include <string>
#include <iostream>
#include <cpr/cpr.h>

/** Constants **/
const cpr::Header USER_AGENT_HEADER = cpr::Header{{"User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.63 Safari/537.36"}};

// URL for testing Internet connectivity
const cpr::Url TEST_URL = cpr::Url{"https://google.com"};
// URL to redirect to portal
const cpr::Url PORTAL_REDIRECT_URL = cpr::Url{"http://1.1.1.1"};
const cpr::Url REGISTRATION_URL = cpr::Url{"https://guest-access.floridapoly.edu:8443/portal/SelfRegistration.action?from=LOGIN"};
const cpr::Url CREATE_ACCOUNT_URL = cpr::Url{"https://guest-access.floridapoly.edu:8443/portal/CreateAccount.action?from=SELF_REGISTRATION"};
const cpr::Url SIGN_ON_URL = cpr::Url{"https://guest-access.floridapoly.edu:8443/portal/SelfRegistrationSuccess.action?from=SELF_REGISTRATION_SUCCESS"};

const cpr::Payload registration_form_data = cpr::Payload{
  {"guestUser.fieldValues.ui_first_name", "A"},
  {"guestUser.fieldValues.ui_last_name", "B"},
  {"guestUser.fieldValues.ui_email_address", "a@a.com"},
  {"guestUser.fieldValues.ui_phone_number", "1234567890"},
  {"guestUser.fieldValues.ui_location", "Florida"},
  {"guestUser.fieldValues.ui_sms_provider", "Global Default"},
  {"aupAccepted", "true"},
};

/**
 * Initialize a session and set some headers
 *
 * @param session The session object
 */
void initialize_session(cpr::Session& session) {
  session.SetRedirect(true);
  session.SetHeader(USER_AGENT_HEADER);
}

/**
 * Send a get request using the session
 *
 * @param session The session object
 * @param url The URL to send the request to
 * @param r The response object to store response of request
 */
void get(cpr::Session& session, const cpr::Url& url, cpr::Response& r) {
  session.SetUrl(url);
  r = session.Get();
}

/**
 * Send a post request using the session
 *
 * @param session The session object
 * @param url The URL to send the request to
 * @param payload The payload of the request
 * @param r The response object to store response of request
 */
void post(cpr::Session& session, const cpr::Url& url, const cpr::Payload& payload, cpr::Response& r) {
  session.SetUrl(url);
  session.SetPayload(payload);
  r = session.Post();
}

/**
 * Send a post request using the session without a payload
 *
 * @param session The session object
 * @param url The URL to send the request to
 * @param r The response object to store response of request
 */
void post_no_payload(cpr::Session& session, const cpr::Url& url, cpr::Response& r) {
  cpr::Payload payload = cpr::Payload{};
  post(session, url, payload, r);
}

/**
 * Check if we have an Internet connection
 *
 * @param session The session object
 */
bool is_connected_to_internet(cpr::Session& session) {
  cpr::Response resp;
  // Send request to test URL
  get(session, TEST_URL, resp);

  // A status code of 0 means request failed
  bool test_1 = resp.status_code != 0;

  // Try sending request to portal redirect URL which should take us to the
  // portal
  get(session, PORTAL_REDIRECT_URL, resp);

  // If request is intercepted by the portal, the Location header will be set
  std::string location_string = resp.header["Location"];
  bool test_2 = location_string.empty();

  // Both tests should pass to verify an Internet connection
  return test_1 && test_2;
}


int main(int argc, char *argv[]) {
  cpr::Session session;
  cpr::Response resp;

  // Initialize session
  initialize_session(session);

  // Check if there is an existing Internet connection
  if (is_connected_to_internet(session)) {
    std::cout << "You already have Internet access!" << std::endl;
    return 0;
  }

  // Get actual portal URL
  get(session, PORTAL_REDIRECT_URL, resp);
  std::string portal_url_string = resp.header["Location"];
  cpr::Url portal_url = cpr::Url{portal_url_string};

  // Go to actual portal
  get(session, portal_url, resp);

  // Go to registration page
  get(session, REGISTRATION_URL, resp);

  // Create an account
  post(session, CREATE_ACCOUNT_URL, registration_form_data, resp);

  // Complete registration
  post_no_payload(session, SIGN_ON_URL, resp);

  std::cout << "You now have Internet access!" << std::endl;

  return 0;
}
