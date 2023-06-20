// export const API_BASE_URL = 'http://3.93.195.0:8000';
export const API_BASE_URL = 'http://174.129.46.97:8000';
export const FILE_BASE_URL = 'http://174.129.46.97:8000';
export const ACCESS_TOKEN = 'token';
export const ACCOUNT_INFO = "accountInfo";

export const OAUTH2_REDIRECT_URI = 'http://localhost:3000/oauth2/redirect'

export const GOOGLE_AUTH_URL = API_BASE_URL + '/oauth2/authorize/google?redirect_uri=' + OAUTH2_REDIRECT_URI;
export const FACEBOOK_AUTH_URL = API_BASE_URL + '/oauth2/authorize/facebook?redirect_uri=' + OAUTH2_REDIRECT_URI;
export const GITHUB_AUTH_URL = API_BASE_URL + '/oauth2/authorize/github?redirect_uri=' + OAUTH2_REDIRECT_URI;

export const errorAlertMessage = "Oops! Something went wrong. Please try again!";
export const sendSuccessMessage = "Verification code has created and sent";
export const verifySuccessMessage = "Youre successfully registered. Please login to continue!";