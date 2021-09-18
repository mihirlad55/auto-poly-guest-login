#include <string>
#include <iostream>
#include <cpr/cpr.h>

const cpr::Header USER_AGENT_HEADER = cpr::Header{{"User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.63 Safari/537.36"}};

const cpr::Url TEST_URL = cpr::Url{"https://google.com"};
const cpr::Url PORTAL_URL = cpr::Url{"http://1.1.1.1"};
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

void initialize_session(cpr::Session& session) {
  session.SetRedirect(true);
  session.SetHeader(USER_AGENT_HEADER);
}

void get(cpr::Session& session, const cpr::Url& url, cpr::Response& r) {
  session.SetUrl(url);
  r = session.Get();
}

void post(cpr::Session& session, const cpr::Url& url, const cpr::Payload& payload, cpr::Response& r) {
  session.SetUrl(url);
  session.SetPayload(payload);
  r = session.Post();
}

void post_no_payload(cpr::Session& session, const cpr::Url& url, cpr::Response& r) {
  cpr::Payload payload = cpr::Payload{};
  post(session, url, payload, r);
}

bool is_connected_to_internet(cpr::Session& session) {
  cpr::Response resp;
  get(session, TEST_URL, resp);

  bool test_1 = resp.status_code != 0;

  get(session, PORTAL_URL, resp);

  std::string location_string = resp.header["Location"];

  bool test_2 = location_string.empty();

  return test_1 && test_2;
}


int main(int argc, char *argv[]) {
  cpr::Session session;
  cpr::Response resp;

  initialize_session(session);

  if (is_connected_to_internet(session)) {
    std::cout << "You already have Internet access!" << std::endl;
    return 0;
  }

  get(session, PORTAL_URL, resp);

  std::string location_string = resp.header["Location"];

  cpr::Url location_url = cpr::Url{location_string};
  get(session, location_url, resp);

  get(session, REGISTRATION_URL, resp);

  post(session, CREATE_ACCOUNT_URL, registration_form_data, resp);

  post_no_payload(session, SIGN_ON_URL, resp);

  std::cout << "You now have Internet access!" << std::endl;

  return 0;
}
