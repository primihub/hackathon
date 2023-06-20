import React from 'react';
import ReactDOM from 'react-dom';
import { HashRouter, Route, Routes } from 'react-router-dom';
import './index.css';
import './index.module.css';
import reportWebVitals from './common/reportWebVitals';
import {Provider} from "react-redux";
import store from './store';
import "lib-flexible";
import MedicalSearch from "./pages/home/Home";
import Login from "./pages/login/Login";
import Register from "./pages/login/Register";
import './common/adapter';

ReactDOM.render(
    <Provider store={store}>
        <HashRouter>
            <Routes>
                <Route path="/" element={<MedicalSearch/>}></Route>
                <Route path="/login" element={<Login/>}></Route>
                <Route path="/register" element={<Register/>}></Route>
            </Routes>
        </HashRouter>
    </Provider>,
  document.getElementById('root')
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
