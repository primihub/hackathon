package com.ischoolbar.programmer.service.Impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.dao.AccountDao;
import com.ischoolbar.programmer.entity.Account;
import com.ischoolbar.programmer.service.AccountService;

@Service
public class AccontServiceImpl implements AccountService {

	@Autowired
	private AccountDao accountDao;
	
	@Override
	public int add(Account account) {
		// TODO Auto-generated method stub
		return accountDao.add(account);
	}

	@Override
	public int edit(Account account) {
		// TODO Auto-generated method stub
		return accountDao.edit(account);
	}

	@Override
	public List<Account> findList(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return accountDao.findList(queryMap);
	}

	@Override
	public Integer getTotal(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return accountDao.getTotal(queryMap);
	}

	@Override
	public int delete(Long id) {
		// TODO Auto-generated method stub
		return accountDao.delete(id);
	}

	@Override
	public Account findByName(String name) {
		// TODO Auto-generated method stub
		return accountDao.findByName(name);
	}

}
