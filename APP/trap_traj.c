#include "trap_traj.h"


int traj_plan(struct TRAJ_s* traj,float t_pos,float s_pos, float t_vel, float s_vel, float time,float max_accel,float max_decel,float max_vel)
{
    float delta_pos = t_pos - s_pos;
    if(t_vel > max_vel) {
        t_vel = max_vel;
    }
    if(ABS(delta_pos) <= 0.1 || time == 0 || t_vel == 0 || max_accel == 0 || max_decel == 0) {
        return -1;
    }
    float vel_diff = t_vel - s_vel;

    traj->distance = ABS(delta_pos);
    traj->dir_flg = delta_pos > 0 ? 1 : -1;

    float max_acc_time = ABS(vel_diff) / max_accel;
    float max_dec_time = ABS(t_vel) / max_decel;

    float max_acc_distance = 0;
    float max_dec_distance = 0;
    float max_keep_distance = 0;

    float more_time = time - (max_acc_time + max_dec_time);
    if(more_time < 0) {
        max_acc_distance = 0.5 * max_accel * SQ(time * max_acc_time/(max_acc_time + max_dec_time));
        max_dec_distance = 0.5 * max_decel * SQ(time * max_dec_time/(max_acc_time + max_dec_time));
        max_keep_distance = 0;
    }
    else {
        max_acc_distance = 0.5 * max_accel * SQ(max_acc_time);
        max_dec_distance = 0.5 * max_decel * SQ(max_dec_time);
        max_keep_distance = t_vel * (time - max_acc_time - max_dec_time);
    }

    if((max_acc_distance + max_dec_distance + max_keep_distance) < traj->distance) {
        return -1;
    }

    if(more_time < (max_acc_time + max_dec_time)) {
        traj->accel_time = time /2;
        traj->decel_time = traj->accel_time;
        traj->cur_accel = 4.0 * (traj->distance - 0.75 * s_vel * time) / SQ(time);
        traj->peak_vel = s_vel + traj->cur_accel * traj->accel_time;

        traj->cur_decel = traj->peak_vel / traj->decel_time;
        if(traj->cur_accel > max_accel || traj->cur_decel > max_decel) {
            return -1;
        }

        traj->keep_distance = 0;
        traj->keep_time = 0;

        traj->acc_distance = 0.5 * traj->cur_accel * SQ(traj->accel_time);
        traj->dec_distance = 0.5 * traj->cur_decel * SQ(traj->decel_time);

        // traj->acc_distance_inc = traj->acc_distance / traj->accel_time * traj->dir_flg;
        // traj->dec_distance_inc = traj->dec_distance / traj->decel_time * traj->dir_flg;
        // traj->keep_distance_inc = 0;

        traj->cur_accel = traj->cur_accel * traj->dir_flg;
        traj->cur_decel = -traj->cur_decel * traj->dir_flg;
    }

    else {
        traj->keep_time = time / 3;
        traj->accel_time = traj->keep_time;
        traj->decel_time = traj->keep_time;

        traj->peak_vel = 3 * traj->distance / (2 * time);
        traj->cur_accel = traj->peak_vel / traj->accel_time;
        traj->cur_decel = traj->peak_vel / traj->decel_time;

        traj->keep_distance = traj->peak_vel * traj->keep_time;
        traj->acc_distance = 0.5 * traj->cur_accel * SQ(traj->accel_time);
        traj->dec_distance = 0.5 * traj->cur_decel * SQ(traj->decel_time);

        // traj->acc_distance_inc = traj->acc_distance / traj->accel_time * traj->dir_flg;
        // traj->dec_distance_inc = traj->dec_distance / traj->decel_time * traj->dir_flg;
        // traj->keep_distance_inc = traj->keep_distance / traj->keep_time * traj->dir_flg;

        traj->cur_accel = traj->peak_vel / traj->accel_time * traj->dir_flg;
        traj->cur_decel = -traj->peak_vel / traj->decel_time * traj->dir_flg;

    }

    traj->input_pos = t_pos;
    traj->vel_set = s_vel;
    traj->stop_flg = false;
    traj->vel_set_point = s_vel;
    traj->pos_set_point = s_pos;


    return 0;
}

void traj_eval(struct TRAJ_s* traj,struct ENCODER_s* encoder,struct CONTROLLER_s* controller,struct USR_CONFIG_s *usr_config)
{
    float deltal_pos = traj->input_pos - encoder->pos;
    if(traj->stop_flg) {
        float target_pos_deltal = traj->input_pos - traj->pos_set_point;
        float deltal_vel = traj->vel_set - traj->vel_set_point;
        float max_accel = controller->input_pos_filter_kp * target_pos_deltal + controller->input_pos_filter_ki * deltal_vel;
        traj->vel_set_point += encoder->time_diff * max_accel;
        traj->pos_set_point += encoder->time_diff * traj->vel_set_point;
        float pos_err = traj->pos_set_point - controller->pos_meas;
        controller->vel_des = usr_config->pos_gain * pos_err;
    }
    else if((deltal_pos * traj->dir_flg) > traj->dec_distance + traj->keep_distance) {
        if(ABS(traj->vel_set) < traj->peak_vel) {
            traj->vel_set += traj->cur_accel * encoder->time_diff;
        }
        else {
            traj->vel_set = traj->peak_vel * traj->dir_flg;
        }
        controller->vel_des = traj->vel_set;
    }
    else if((deltal_pos * traj->dir_flg) > traj->dec_distance) {
        traj->vel_set = traj->peak_vel * traj->dir_flg;
        controller->vel_des = traj->vel_set;
    }
    else {
        traj->vel_set += traj->cur_decel * encoder->time_diff;
        controller->vel_des = traj->vel_set;
        if(ABS(traj->vel_set) < usr_config->target_velcity_window || (deltal_pos * traj->dir_flg) < usr_config->target_position_window) {
            traj->stop_flg = true;
            traj->vel_set = 0;
            traj->pos_set_point = encoder->pos;
            traj->vel_set_point = encoder->vel;
        }  
    }
}
