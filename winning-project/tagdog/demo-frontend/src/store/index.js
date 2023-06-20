import { configureStore } from '@reduxjs/toolkit';
// import roleReducer from '../features/role/roles';
import accountReducer from '../features/account/account';

export default configureStore({
    reducer: {
        // role: roleReducer,
        account: accountReducer,
    },
})
