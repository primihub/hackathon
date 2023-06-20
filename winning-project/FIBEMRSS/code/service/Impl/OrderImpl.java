package com.ischoolbar.programmer.service.Impl;

import java.util.List;
import java.util.Map;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.ischoolbar.programmer.dao.OrderDao;
import com.ischoolbar.programmer.entity.Order;
import com.ischoolbar.programmer.entity.OrderItem;
import com.ischoolbar.programmer.service.OrderService;

@Service
public class OrderImpl implements OrderService {

	@Autowired
	private OrderDao orderDao;
	@Override
	public int add(Order order) {
		// TODO Auto-generated method stub
		return orderDao.add(order);
	}

	@Override
	public int edit(Order Order) {
		// TODO Auto-generated method stub
		return orderDao.edit(Order);
	}

	@Override
	public List<Order> findList(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return orderDao.findList(queryMap);
	}

	@Override
	public Integer getTotal(Map<String, Object> queryMap) {
		// TODO Auto-generated method stub
		return orderDao.getTotal(queryMap);
	}

	@Override
	public int delete(Long id) {
		// TODO Auto-generated method stub
		return orderDao.delete(id);
	}

	@Override
	public int addItem(OrderItem orderItem) {
		// TODO Auto-generated method stub
		return orderDao.addItem(orderItem);
	}

	@Override
	public int elter(Long id) {
		// TODO Auto-generated method stub
		return orderDao.elter(id);
	}

}
