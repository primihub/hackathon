package com.ischoolbar.programmer.dao.admin;

import java.util.List;
import java.util.Map;

import org.springframework.stereotype.Repository;

import com.ischoolbar.programmer.entity.amdin.User;

/**
 * User用户Dao
 * @author lenovo
 *
 */
@Repository
public interface UserDao {
	public User findByUsername(String username);
	public int add(User user);
	public int edit(User user);
	public int delete(String ids);
	public List<User> findList(Map<String, Object> quertMap);
	public int getTotal(Map<String, Object> queryMap);
	public int editPassword(User user);
}
