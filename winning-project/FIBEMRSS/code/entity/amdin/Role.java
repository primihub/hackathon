package com.ischoolbar.programmer.entity.amdin;

import org.springframework.stereotype.Component;


/**
 * 角色role实体
 * @author lenovo
 *
 */
@Component
public class Role {
	private Long id;
	private String name;
	private String remark;//角色备注
	public Long getId() {
		return id;
	}
	public void setId(Long id) {
		this.id = id;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getRemark() {
		return remark;
	}
	public void setRemark(String remark) {
		this.remark = remark;
	}
	

}
