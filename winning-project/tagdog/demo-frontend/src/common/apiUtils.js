import { API_BASE_URL, ACCESS_TOKEN } from '../constants/url';

const request = (options) => {
  const headers = new Headers({
    'Content-Type': 'application/json',
  })

  if (localStorage.getItem(ACCESS_TOKEN)) {
    headers.append('Authorization', 'Bearer ' + localStorage.getItem(ACCESS_TOKEN))
  }

  const defaults = { headers: headers };
  options = Object.assign({}, defaults, options);

  return fetch(options.url, options)
    .then(response => response.json().then(json => {
      if (!response.ok) {
        return Promise.reject(json);
      }
      return json;
    })
    );
};

const concatGetParams = ({ relativePath, params }) => {
  let result = "";
  let item;
  for (item in params) {
    if (params[item] && String(params[item])) {
      result += `&${item}=${params[item]}`;
    }
  }
  return API_BASE_URL + relativePath + "?" + result.slice(1);
}

export function register(params) {
    return request({
        url: API_BASE_URL + "/api/v1/register",
        method: 'POST',
        body: JSON.stringify(params)
      });
}

export function login(params) {
    return request({
        url: API_BASE_URL + "/api/v1/login",
        method: 'POST',
        body: JSON.stringify(params)
    });
}

export function getCurrentUser(params) {
    return request({
        url: API_BASE_URL + "/api/v1/getCurrentUser",
        method: 'POST',
        body: JSON.stringify(params)
    });
}

export function getUMRByKeyword(params) {
  return request({
    url: API_BASE_URL + "/api/v1/getUMR-keyword",
    method: 'POST',
    body: JSON.stringify(params)
  });
}

export function queryUserGrant(params) {
  return request({
    url: API_BASE_URL + "/api/v1/queryGrant",
    method: 'POST',
    body: JSON.stringify(params)
  });
}

export function addNewMR(params) {
  return request({
    url: API_BASE_URL + "/api/v1/addUMR",
    method: 'POST',
    body: JSON.stringify(params)
  });
}

export function queryGrant(params) {
  return request({
    url: API_BASE_URL + "/api/v1/queryGrant",
    method: 'POST',
    body: JSON.stringify(params)
  });
}

export function grantMR(params) {
  return request({
    url: API_BASE_URL + "/api/v1/grant",
    method: 'POST',
    body: JSON.stringify(params)
  });
}
