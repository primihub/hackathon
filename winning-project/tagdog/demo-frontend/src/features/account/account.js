import { createSlice } from '@reduxjs/toolkit';
import { ACCOUNT_INFO } from '../../constants/url';

const defaultAccount = localStorage.getItem(ACCOUNT_INFO);

export const accountSlice = createSlice({
    name: 'account',
    initialState: {
        value: JSON.parse(defaultAccount),
    },
    reducers: {
        switchAccount: (state, action) => {
            console.log(state, action.payload.account)
            state.value = action.payload.account
        },
    },
})

// Action creators are generated for each case reducer function
export const { switchAccount } = accountSlice.actions

export default accountSlice.reducer
